#include <cstdio>
#include <cstring>
#include "tarball.h"

using namespace std;

// SEE ALSO:
// http://www.mkssoftware.com/docs/man4/tar.4.asp
// http://en.wikipedia.org/wiki/Tar_%28file_format%29
// http://cvs.savannah.gnu.org/viewvc/*checkout*/tar/tar/src/tar.h?content-type=text/plain
// https://github.com/lindenb/cclindenb/tree/master/src/core/lindenb/io
//
/****
{				// byte offset
  char name[100];		//   0
  char mode[8];			// 100
  char uid[8];			// 108
  char gid[8];			// 116
  char size[12];		// 124
  char mtime[12];		// 136
  char chksum[8];		// 148
  char typeflag;		// 156
  char linkname[100];		// 157
  char magic[6];		// 257
  char version[2];		// 263
  char uname[32];		// 265
  char gname[32];		// 297
  char devmajor[8];		// 329
  char devminor[8];		// 337
  char prefix[155];		// 345
                                // 500
};
****/

#ifdef _MSC_VER

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#define snprintf c99_snprintf

#endif // _MSC_VER

struct TARHeader {
    char s_name[100];
    char s_mode[8];
    char s_uid[8];
    char s_gid[8];
    char s_size[12];
    char s_mtime[12];
    char s_csum[8];
    char s_ftype[1];
    char s_lname[100];
//    char s_magic[6];
//    char s_version[2];
    char s_ustar[8];
    char s_uname[32];
    char s_gname[32];
    char s_devmajor[8];
    char s_devminor[8];
    char s_prefix[155];
    char s_pad[12];

    TARHeader(const char *name, quint64 size) {
        init();
        setup(name, size);
    }

    TARHeader() {
        init();
    }

    void init() {
        const size_t headerSize = sizeof(TARHeader);
        Q_ASSERT (headerSize == 512);
        memset((void*)this, 0, headerSize);
    }

    void setup(const char *name, quint64 size) {
        // base tar
        snprintf(s_name, 100, "%s", name);
        snprintf(s_mode, 8, "%07o", (unsigned int)0644);
//        snprintf(s_uid, 8, "%07o", (unsigned int)getuid());
//        snprintf(s_gid, 8, "%07o", (unsigned int)getgid());
        snprintf(s_size, 12, "%011lo", (long unsigned int)size);
//      snprintf(s_mtime, 12, "%011lo", (long unsigned int)time(NULL));
        snprintf(s_mtime, 12, "11715417351");
        s_ftype[0] = '0';
        // ustar
#ifdef _MSC_VER
        sprintf_s(s_ustar, "ustar  ");
#else
        sprintf(s_ustar, "ustar  ");
#endif
        //sprintf(s_version, "00");

//        char *login = getlogin();
//        if (login) {
//            snprintf(s_uname, 32, "%s", login);
//            snprintf(s_gname, 32, "%s", login);
//        } else {
//            static const char stub[] = "u";
//            snprintf(s_uname, 32, "%s", stub);
//            snprintf(s_gname, 32, "%s", stub);
//        }
        checksum();
    }

    void checksum() {
        quint32 s = 0;
        memset(s_csum, ' ', 8);
        const unsigned char *pStart = (const unsigned char *)this;
        const unsigned char *pEnd = pStart + sizeof(*this);

        for (const unsigned char *p = pStart; p < pEnd; ++p) {
            s += *p & 0xFF;
        }
        snprintf(s_csum, 8, "%06o", s);
        s_csum[6] = 0;
        s_csum[7] = ' '; //just for sure =)
    }

    QByteArray toByteArray() const {
        return QByteArray((const char*)this, sizeof(*this));
    }
};

QByteArray tarFile(const QString& name, const QByteArray& data) {
    int padding = (512 - (data.size() % 512)) % 512;
    QByteArray r;
    r.reserve(sizeof(TARHeader) + data.size() + padding);
    r.append(TARHeader(qPrintable(name), data.size()).toByteArray());
    r.append(data);
    if (padding) {
        r.append(QByteArray(padding, '\0'));
    }
    return r;
}

/*
  Name  : CRC-32
  Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 байт (2 147 483 647 бит) - обнаружение
   одинарных, двойных, пакетных и всех нечетных ошибок

   see also: http://www.gzip.org/zlib/rfc-gzip.html
*/
quint32 Crc32(const unsigned char *buf, size_t len)
{
    quint32 crc_table[256];
    quint32 crc; int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--)
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}

quint32 Crc32(const QByteArray& block) {
    return Crc32((const unsigned char *)(block.constBegin()), block.size());
}

// http://www.qtforum.org/article/27065/
// how-to-use-zlib-data-created-by-qcompress-in-gzip-file.html
//////////////////////////////////////////////////////////////////////////

QByteArray gzipFile(const QByteArray& data) {
    QByteArray z = qCompress(data,9);
    z.remove(0, 6); // Remove Zlib header
    z.remove(z.size() - 4, 4); // Remove Zlib trailer

    //this is valid gzip header - magic number ,
    // deflate compression, no extra fields,
    // no timestamp present, maximum compression extra flag and 'other' os
    const unsigned char gzipheader[10]={0x1f,0x8b,8,0,0,0,0,0,2,255};
    quint32 crc = Crc32(data);
    quint32 len = data.size();

    QByteArray zip;
    zip.append((const char*)gzipheader, 10);
    zip.append(z);
    zip.append((char *)&crc, 4);
    zip.append((char *)&len, 4);
    return zip;
}

/////////////////////////////////////////////////////////////////////
// Tarball
/////////////////////////////////////////////////////////////////////

Tarball::Tarball() : _tar(NULL) {
}

Tarball::~Tarball() {
    close();
}

bool Tarball::open(const QString& fname) {
    if (isOpened()) return true;
    _fname = fname;
    _tar = new QFile(_fname);
    if (!_tar->open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning("tarball %s was not been opened", qPrintable(_fname));
        delete _tar;
        _tar = NULL;
        return false;
    }
    return true;
}

void Tarball::close() {
    if (!isOpened()) return;
//    QByteArray z(4096, '\0');
    QByteArray z(gzipFile(QByteArray(4096, '\0')));
    _tar->write(z); // 1
    _tar->write(z); // 2
    _tar->flush();
    _tar->close();
    delete _tar;
    _tar = NULL;
}

bool Tarball::isOpened() const {
    return (_tar != NULL);
}


bool Tarball::add(const QString& name, const QByteArray& data) {
    if (!isOpened()) {
        return false;
    }
//    _tar->write(tarFile(name, data));
    _tar->write(gzipFile(tarFile(name, data)));
    _tar->flush();
    return true;
}
