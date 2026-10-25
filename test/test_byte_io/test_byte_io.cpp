#include <unity.h>

#include "protocol/common/byte_reader.hpp"
#include "protocol/common/byte_writer.hpp"

using can_rail::protocol::ByteReader;
using can_rail::protocol::ByteWriter;
using can_rail::protocol::ProtocolError;

void test_reader_reads_unsigned_values_in_big_endian_order() {
    const std::uint8_t bytes[] = {0x12, 0x34, 0x56, 0x78, 0x9A};
    ByteReader reader(bytes, sizeof(bytes));

    const auto first = reader.readU8();
    const auto second = reader.readU16BigEndian();
    const auto third = reader.readU16BigEndian();

    TEST_ASSERT_TRUE(first);
    TEST_ASSERT_EQUAL_UINT8(0x12, first.value());
    TEST_ASSERT_TRUE(second);
    TEST_ASSERT_EQUAL_UINT16(0x3456, second.value());
    TEST_ASSERT_TRUE(third);
    TEST_ASSERT_EQUAL_UINT16(0x789A, third.value());
    TEST_ASSERT_EQUAL_UINT32(0, reader.remaining());
}

void test_reader_reports_short_buffer_without_advancing() {
    const std::uint8_t bytes[] = {0x12};
    ByteReader reader(bytes, sizeof(bytes));

    const auto result = reader.readU16BigEndian();

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::BufferTooShort),
                      static_cast<int>(result.error()));
    TEST_ASSERT_EQUAL_UINT32(1, reader.remaining());
}

void test_writer_writes_unsigned_values_in_big_endian_order() {
    std::uint8_t bytes[7]{};
    ByteWriter writer(bytes, sizeof(bytes));

    TEST_ASSERT_TRUE(writer.writeU8(0x12));
    TEST_ASSERT_TRUE(writer.writeU16BigEndian(0x3456));
    TEST_ASSERT_TRUE(writer.writeU32BigEndian(0x789ABCDE));
    TEST_ASSERT_EQUAL_UINT32(7, writer.size());
    TEST_ASSERT_EQUAL_UINT32(0, writer.remaining());
    TEST_ASSERT_EQUAL_UINT8(0x12, bytes[0]);
    TEST_ASSERT_EQUAL_UINT8(0x34, bytes[1]);
    TEST_ASSERT_EQUAL_UINT8(0x56, bytes[2]);
    TEST_ASSERT_EQUAL_UINT8(0x78, bytes[3]);
    TEST_ASSERT_EQUAL_UINT8(0x9A, bytes[4]);
    TEST_ASSERT_EQUAL_UINT8(0xBC, bytes[5]);
    TEST_ASSERT_EQUAL_UINT8(0xDE, bytes[6]);
}

void test_writer_reports_full_buffer_without_advancing() {
    std::uint8_t bytes[1]{};
    ByteWriter writer(bytes, sizeof(bytes));

    const auto result = writer.writeU16BigEndian(0x1234);

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(static_cast<int>(ProtocolError::BufferFull),
                      static_cast<int>(result.error()));
    TEST_ASSERT_EQUAL_UINT32(0, writer.size());
    TEST_ASSERT_EQUAL_UINT32(1, writer.remaining());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_reader_reads_unsigned_values_in_big_endian_order);
    RUN_TEST(test_reader_reports_short_buffer_without_advancing);
    RUN_TEST(test_writer_writes_unsigned_values_in_big_endian_order);
    RUN_TEST(test_writer_reports_full_buffer_without_advancing);
    return UNITY_END();
}
