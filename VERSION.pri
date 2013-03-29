BUILD_HASH=$$system("git show --name-only --format='%h' | head -n1")
BUILD_TSTAMP=$$system("git show --name-only --format='%ci' | head -n1 | awk '{print $1, $2}'")
BUILD_SUBJ=$$system("git show --name-only --format='%s' | head -n1")

