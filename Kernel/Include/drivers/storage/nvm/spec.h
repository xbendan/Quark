#include <mixins/std/c++types.h>

#define NVME_CAP_CMBS (1 << 57)           // Controller memory buffer supported
#define NVME_CAP_PMRS (1 << 56)           // Persistent memory region supported
#define NVME_CAP_BPS (1 << 45)            // Boot partition support
#define NVME_CAP_NVM_CMD_SET (1UL << 37)  // NVM command set supported
#define NVME_CAP_NSSRS (1UL << 36)        // NVM subsystem reset supported
#define NVME_CAP_CQR (1 << 16)            // Contiguous Queues Required

#define NVME_CAP_MPS_MASK 0xfU
#define NVME_CAP_MPSMAX(x) \
    ((x >> 52) & NVME_CAP_MPS_MASK)  // Max supported memory page size (2 ^ (12
                                     // + MPSMAX))
#define NVME_CAP_MPSMIN(x) \
    ((x >> 48) & NVME_CAP_MPS_MASK)  // Min supported memory page size (2 ^ (12
                                     // + MPSMIN))

#define NVME_CAP_DSTRD_MASK 0xfU
#define NVME_CAP_DSTRD(x)                                               \
    (((x) >> 32)                                                        \
     & NVME_CAP_DSTRD_MASK)  // Doorbell stride (2 ^ (2 + DSTRD)) bytes

#define NVME_CAP_MQES_MASK 0xffffU
#define NVME_CAP_MQES(x)                                           \
    ((x) & NVME_CAP_MQES_MASK)  // Maximum queue entries supported

#define NVME_CFG_MPS_MASK 0xfUL
#define NVME_CFG_MPS(x)                                                        \
    (((x) & NVME_CFG_MPS_MASK)                                                 \
     << 7)                        // Host memory page size (2 ^ (12 + MPSMIN))
#define NVME_CFG_CSS_MASK 0b111U  // Command set selected
#define NVME_CFG_CSS(x) (((x) & NVME_CFG_CSS_MASK) << 4)
#define NVME_CFG_ENABLE (1 << 0)

#define NVME_CFG_DEFAULT_IOCQES (4 << 20)  // 16 bytes so log2(16) = 4
#define NVME_CFG_DEFAULT_IOSQES (6 << 16)  // 64 bytes so log2(64) = 6

#define NVME_CSTS_FATAL (1 << 1)
#define NVME_CSTS_READY                                                     \
    (1 << 0)  // Set to 1 when the controller is ready to accept submission
              // queue doorbell writes
#define NVME_CSTS_NSSRO (1 << 4)  // NVM Subsystem reset occurred

#define NVME_AQA_AQS_MASK 0xfffU  // Admin queue size mask
#define NVME_AQA_ACQS(x)                                              \
    (((x) & NVME_AQA_AQS_MASK) << 16)  // Admin completion queue size
#define NVME_AQA_ASQS(x)                                             \
    (((x) & NVME_AQA_AQS_MASK) << 0)  // Admin submission queue size

#define NVME_NSSR_RESET_VALUE 0x4E564D65  // "NVME", initiates a reset

namespace NVMe {

    struct CommandBase {
        struct {
            u32 _opcode: 8;
            u32 _fuse: 2;
            u32 _reserved0: 4;
            u32 _psdt: 2;
            u32 _cid: 16;
        } __attribute__((packed));

        u32 _nsid;
        u64 _reserved1;
        u64 _mptr;
        u64 _prp1;
        u64 _prp2;
    };

    struct Identify : public CommandBase {
        enum {
            CNS_NAMESPACE         = 0,
            CNS_CONTROLLER        = 1,
            CNS_ACTIVE_NAMESPACES = 2,
        };

        struct {
            u32 _cns;
            u32 _reserved0;
            u64 _nsid;
        } __attribute__((packed));

        u32 _nvmSetId;
    };

    struct CreateIoCompletionQueue : public CommandBase {
        struct {
            u32 _queueId: 16;
            u32 _queueSize: 16;
        } __attribute__((packed));

        struct {
            u32 _contiguous: 1;
            u32 _withInterrupt: 1;
            u32 _reserved0: 14;
            u32 _intVector: 16;
        } __attribute__((packed));
    };

    struct CreateIoSubmissionQueue : public CommandBase {
        struct {
            u32 _queueId: 16;
            u32 _queueSize: 16;
        } __attribute__((packed));

        struct {
            u32 _contiguous: 1;
            u32 _priority: 2;
            u32 _reserved0: 13;
            u32 _cqId: 16;
        } __attribute__((packed));
    };

    struct DeleteIoCompletionQueue : public CommandBase {
        struct {
            u32 _queueId: 16;
            u32 _reserved0: 16;
        } __attribute__((packed));
    };

    struct SetFeatures {
        enum {
            FeatureIdNumerOfQueues = 0x7,
        };

        struct {
            u32 _featId: 8;
            u32 _reserved0: 23;
            u32 _save: 1;
        } __attribute__((packed));

        u32 _dw11;
        u32 _dw12;
        u32 _dw13;
    };

    struct Write {
        u64 _startingLba;

        struct {
            u32 _blockNum: 16;
            u32 _reserved0: 4;
            u32 _drType: 4;
            u32 _reserved1: 2;
            u32 _prInfo: 4;
            u32 _forceAccess: 1;
            u32 _limitedRetry: 1;
        };
    };

    struct Read : public Write {
        u64 _startingLba;

        struct {
            u32 _blockNum: 16;
            u32 _reserved0: 10;
            u32 _prInfo: 4;
            u32 _forceAccess: 1;
            u32 _limitedRetry: 1;
        };
    };

}  // namespace NVMe