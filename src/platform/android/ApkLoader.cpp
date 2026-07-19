#include "engine/file/FileInterfaceAndroid.h"

extern "C" int loadAPK(const char *path) {
    void *za = zip_open(path, 0, 0);
    *FileInterfaceAndroid::gZipMain = za;
    if (za == 0)
        return 0;

    int count = zip_get_num_files((struct zip *) za);
    for (int i = 0; i < count; ++i) {
        if (zip_get_name((struct zip *) *FileInterfaceAndroid::gZipMain, i, 0) == 0)
            break;
    }
    return 1;
}

extern "C" int loadAPKAndZip(const char *apkPath, const char *patchPath) {
    void *apk = zip_open(apkPath, 0, 0);
    *FileInterfaceAndroid::gZipMain = apk;

    void *patch = zip_open(patchPath, 0, 0);
    *FileInterfaceAndroid::gZipPatch = patch;

    return apk != 0 && patch != 0;
}
