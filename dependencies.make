$(BUILD)/algorithm-test.o: rs-core/algorithm-test.cpp rs-core/algorithm.hpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/array-map-test.o: rs-core/array-map-test.cpp rs-core/array-map.hpp rs-core/common.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/blob-test.o: rs-core/blob-test.cpp rs-core/blob.hpp rs-core/common.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/cache-test.o: rs-core/cache-test.cpp rs-core/cache.hpp rs-core/common.hpp rs-core/unit-test.hpp
$(BUILD)/channel-test.o: rs-core/channel-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/common-test.o: rs-core/common-test.cpp rs-core/common.hpp rs-core/unit-test.hpp
$(BUILD)/compact-array-test.o: rs-core/compact-array-test.cpp rs-core/common.hpp rs-core/compact-array.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/digest-test.o: rs-core/digest-test.cpp rs-core/common.hpp rs-core/digest.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/encoding-test.o: rs-core/encoding-test.cpp rs-core/common.hpp rs-core/encoding.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/file-test.o: rs-core/file-test.cpp rs-core/common.hpp rs-core/file.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/float-test.o: rs-core/float-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp
$(BUILD)/grid-test.o: rs-core/grid-test.cpp rs-core/common.hpp rs-core/grid.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp
$(BUILD)/index-table-test.o: rs-core/index-table-test.cpp rs-core/common.hpp rs-core/index-table.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/io-test.o: rs-core/io-test.cpp rs-core/common.hpp rs-core/file.hpp rs-core/io.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/kwargs-test.o: rs-core/kwargs-test.cpp rs-core/common.hpp rs-core/kwargs.hpp rs-core/unit-test.hpp
$(BUILD)/meta-test.o: rs-core/meta-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/mp-integer-test.o: rs-core/mp-integer-test.cpp rs-core/common.hpp rs-core/mp-integer.hpp rs-core/rational.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/net-test.o: rs-core/net-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/net.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/optional-test.o: rs-core/optional-test.cpp rs-core/common.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/process-test.o: rs-core/process-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/file.hpp rs-core/io.hpp rs-core/optional.hpp rs-core/process.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/random-test.o: rs-core/random-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/random.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp
$(BUILD)/rational-test.o: rs-core/rational-test.cpp rs-core/common.hpp rs-core/rational.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/scale-map-test.o: rs-core/scale-map-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/scale-map.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp
$(BUILD)/signal-test.o: rs-core/signal-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/signal.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/stack-test.o: rs-core/stack-test.cpp rs-core/common.hpp rs-core/stack.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/statistics-test.o: rs-core/statistics-test.cpp rs-core/common.hpp rs-core/statistics.hpp rs-core/unit-test.hpp
$(BUILD)/string-test.o: rs-core/string-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/unit-test.hpp
$(BUILD)/table-test.o: rs-core/table-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/table.hpp rs-core/unit-test.hpp
$(BUILD)/terminal-test.o: rs-core/terminal-test.cpp rs-core/common.hpp rs-core/file.hpp rs-core/float.hpp rs-core/io.hpp rs-core/string.hpp rs-core/terminal.hpp rs-core/time.hpp rs-core/unit-test.hpp rs-core/vector.hpp
$(BUILD)/time-test.o: rs-core/time-test.cpp rs-core/common.hpp rs-core/time.hpp rs-core/unit-test.hpp
$(BUILD)/unit-test.o: rs-core/unit-test.cpp rs-core/common.hpp rs-core/unit-test.hpp
$(BUILD)/uuid-test.o: rs-core/uuid-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/random.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/uuid.hpp rs-core/vector.hpp
$(BUILD)/variant-test.o: rs-core/variant-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/variant.hpp
$(BUILD)/vector-test.o: rs-core/vector-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp
$(BUILD)/zlib-test.o: rs-core/zlib-test.cpp rs-core/common.hpp rs-core/file.hpp rs-core/float.hpp rs-core/io.hpp rs-core/random.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp rs-core/vector.hpp rs-core/zlib.hpp
ifeq ($(LIBTAG),cygwin)
    LDLIBS += -lcrypto
endif
ifeq ($(LIBTAG),linux)
    LDLIBS += -lcrypto
endif
ifeq ($(LIBTAG),msvc)
    LDLIBS += advapi32.lib shell32.lib ws2_32.lib zlib.lib
endif
