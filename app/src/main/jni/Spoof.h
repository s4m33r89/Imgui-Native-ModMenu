#include "Includes.h"
#include "fake_dlfcn.h"
#include "plthook.h"

// ============================================================================================= //
const char *namesToHide[] = {"linker", "libavaritia.so"};

const char *isNameInBlackList(const char *in) {
    if (!in)
        return 0;

    for (int i = 0; i < (sizeof(namesToHide) / sizeof(namesToHide[0])); i++) {
        const char *found = strstr(in, namesToHide[i]);
        if (found) {
            return found;
        }
    }
    return 0;
}

char pkgName[64];

// ============================================================================================= //
int _open(const char *pathname, int flags, mode_t mode) {
    if (isNameInBlackList(pathname)) {
        return -1;
    }

    if (strstr(pathname, pkgName)) {
        if (strstr(pathname, ".apk")) {
            std::string s = "/data/data/";
            s += pkgName;
            s += "/lib/libtia.so";
            pathname = s.c_str();
        }
    }

    return open(pathname, flags, mode);
}

int _creat(const char *pathname, int flags) {
    if (isNameInBlackList(pathname)) {
        return -1;
    }

    if (strstr(pathname, pkgName)) {
        if (strstr(pathname, ".apk")) {
            std::string s = "/data/data/";
            s += pkgName;
            s += "/lib/libtia.so";
            pathname = s.c_str();
        }
    }

    return creat(pathname, flags);
}

// ============================================================================================= //
FILE *_fopen(const char *pathname, const char *mode) {
    if (isNameInBlackList(pathname)) {
        return 0;
    }

    if (strstr(pathname, pkgName)) {
        if (strstr(pathname, ".apk")) {
            std::string s = "/data/data/";
            s += pkgName;
            s += "/lib/libtia.so";
            pathname = s.c_str();
        }
    }

    return fopen(pathname, mode);
}

// ============================================================================================= //
ssize_t _read(int fd, void *buf, size_t count) {
    auto result = read(fd, buf, count);

    if (result > 0) {
        uint8_t *buff = (uint8_t *) buf;
        for (int i = 0; i < result; i++) {
            for (int j = 0; j < (sizeof(namesToHide) / sizeof(namesToHide[0])); j++) {
                auto name = namesToHide[j];
                if (i <= (result - strlen(name))) {
                    if (memcmp(&buff[i], name, strlen(name)) == 0) {
                        memset(&buff[i], '.', strlen(name));
                    }
                }
            }
        }
    }
    return result;
}

// ============================================================================================= //
size_t _fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    auto result = fread(ptr, size, nmemb, stream);

    if (result > 0) {
        uint8_t *buff = (uint8_t *) ptr;
        for (int i = 0; i < result; i++) {
            for (int j = 0; j < (sizeof(namesToHide) / sizeof(namesToHide[0])); j++) {
                auto name = namesToHide[j];
                if (i <= (result - strlen(name))) {
                    if (memcmp(&buff[i], name, strlen(name)) == 0) {
                        memset(&buff[i], '.', strlen(name));
                    }
                }
            }
        }
    }

    return result;
}

// ============================================================================================= //
int _access(const char *pathname, int mode) {
    if (isNameInBlackList(pathname)) {
        return -1;
    }

    if (strstr(pathname, pkgName)) {
        if (strstr(pathname, ".apk")) {
            std::string s = "/data/data/";
            s += pkgName;
            s += "/lib/libtia.so";
            pathname = s.c_str();
        }
    }

    return access(pathname, mode);
}

// ============================================================================================= //
int _stat(const char *pathname, struct stat *statbuf) {
    if (isNameInBlackList(pathname)) {
        return -1;
    }

    if (strstr(pathname, pkgName)) {
        if (strstr(pathname, ".apk")) {
            std::string s = "/data/data/";
            s += pkgName;
            s += "/lib/libtia.so";
            pathname = s.c_str();
        }
    }

    return stat(pathname, statbuf);
}

int _lstat(const char *pathname, struct stat *statbuf) {
    if (isNameInBlackList(pathname)) {
        return -1;
    }

    if (strstr(pathname, pkgName)) {
        if (strstr(pathname, ".apk")) {
            std::string s = "/data/data/";
            s += pkgName;
            s += "/lib/libtia.so";
            pathname = s.c_str();
        }
    }

    return lstat(pathname, statbuf);
}

// ============================================================================================= //
char *_fgets(char *dest, int size, FILE *stream) {
    auto ret = fgets(dest, size, stream);
    check:
    if (ret) {
        if (isNameInBlackList(ret)) {
            ret = fgets(dest, size, stream);
            goto check;
        }
    }

    return ret;
}

// ============================================================================================= //
DIR *_opendir(const char *name) {
    if (isNameInBlackList(name)) {
        return 0;
    }
    return opendir(name);
}

struct dirent *_readdir(DIR *dirp) {
    auto ret = readdir(dirp);
    check:
    if (ret) {
        if (isNameInBlackList(ret->d_name)) {
            ret = readdir(dirp);
            goto check;
        }
    }
    return ret;
}

// ============================================================================================= //


// ============================================================================================= //

