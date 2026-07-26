enum DeviceState {
    DEV_UNKNOWN,
    DEV_OK,
    DEV_ERROR,
    DEV_RECOVERY
};

struct DeviceStatus {
    DeviceState state;
    int lastError;
    uint32_t lastOk;
    uint32_t lastCheck;
};
