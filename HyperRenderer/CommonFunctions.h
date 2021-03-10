template <typename T>
inline void SafeRelease(T *&p) { if (nullptr != p) { p->Release(); p = nullptr; } }