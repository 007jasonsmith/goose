#ifndef KLIB_LIMITS_H_
#define KLIB_LIMITS_H_

const int32 kMaxInt32 = int32(2147483647);
const int32 kMinInt32 = int32(-2147483648);

const int64 kMaxInt64 = int64(9223372036854775807LL);
const int64 kMinInt64 = int64(-9223372036854775807LL - 1LL);

const uint32 kMaxUInt32 = uint32(4294967295);
const uint32 kMinUInt32 = uint32(0);

const uint64 kMaxUInt64 = uint64(0xFFFFFFFFFFFFFFFFULL);
const uint64 kMinUInt64 = uint64(0ULL);

// Numerical limits.
template<typename T>
struct Limits {
  const static T Max;
  const static T Min;
  const static bool IsSigned;
};

template<>
struct Limits<int8> {
  const static int8 Max = int8(127);
  const static int8 Min = int8(-128);
  const static bool IsSigned = true;
};

template<>
struct Limits<int16> {
  const static int16 Max = int16(32767);
  const static int16 Min = int16(-32768);
  const static bool IsSigned = true;
};

template<>
struct Limits<int32> {
  const static int32 Max = kMaxInt32;
  const static int32 Min = kMinInt32;
  const static bool IsSigned = true;
};

template<>
struct Limits<int64> {
  const static int64 Max = kMaxInt64;
  const static int64 Min = kMinInt64;
  const static bool IsSigned = true;
};

template<>
struct Limits<uint8> {
  const static uint8 Max = int8(255);
  const static uint8 Min = int8(0);
  const static bool IsSigned = false;
};

template<>
struct Limits<uint16> {
  const static uint16 Max = uint16(65535);
  const static uint16 Min = uint16(0);
  const static bool IsSigned = false;
};

template<>
struct Limits<uint32> {
  const static uint32 Max = kMaxUInt32;
  const static uint32 Min = kMinUInt32;
  const static bool IsSigned = false;
};

template<>
struct Limits<uint64> {
  const static uint64 Max = kMaxUInt64;
  const static uint64 Min = kMinUInt64;
  const static bool IsSigned = false;
};

#endif  // KLIB_LIMITS_H_
