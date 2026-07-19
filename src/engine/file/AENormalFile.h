#ifndef GOF2_AENORMALFILE_H
#define GOF2_AENORMALFILE_H
#include "AELowLevelFile.h"
#include "FileInterface.h"

class FileInterface;


// A plain file opened through the platform FileInterface; every operation is
// forwarded straight to the underlying interface.
class AENormalFile : public AELowLevelFile {
public:
    FileInterface *file;

    AENormalFile(FileInterface *file);

    ~AENormalFile() override;

    uint32_t Write(uint32_t bytes, void *buffer) override;

    uint32_t Read(uint32_t bytes, void *buffer) override;

    uint32_t Skip(uint32_t bytes) override;

    uint32_t Release() override;

    uint32_t GetFileSize() override;
};
#endif
