$(BUILD)/algorithm-test.o: rs-core/algorithm-test.cpp rs-core/algorithm.hpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/array-map-test.o: rs-core/array-map-test.cpp rs-core/array-map.hpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/blob-test.o: rs-core/blob-test.cpp rs-core/blob.hpp rs-core/common.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/blob.o: rs-core/blob.cpp rs-core/blob.hpp rs-core/common.hpp rs-core/digest.hpp rs-core/string.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/bounded-array-test.o: rs-core/bounded-array-test.cpp rs-core/bounded-array.hpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/cache-test.o: rs-core/cache-test.cpp rs-core/cache.hpp rs-core/common.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/channel-test.o: rs-core/channel-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/channel.o: rs-core/channel.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/common-test.o: rs-core/common-test.cpp rs-core/common.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/common.o: rs-core/common.cpp rs-core/common.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/compact-array-test.o: rs-core/compact-array-test.cpp rs-core/common.hpp rs-core/compact-array.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/digest-test.o: rs-core/digest-test.cpp rs-core/common.hpp rs-core/digest.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/dso-test.o: rs-core/dso-test.cpp rs-core/common.hpp rs-core/dso.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/encoding-test.o: rs-core/encoding-test.cpp rs-core/common.hpp rs-core/encoding.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/file-system-test.o: rs-core/file-system-test.cpp rs-core/common.hpp rs-core/file-system.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/regex.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/file-system.o: rs-core/file-system.cpp rs-core/common.hpp rs-core/file-system.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/regex.hpp /usr/local/include/unicorn/segment.hpp /usr/local/include/unicorn/string.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/fixed-binary-test.o: rs-core/fixed-binary-test.cpp rs-core/common.hpp rs-core/fixed-binary.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/statistics.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/float-test.o: rs-core/float-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/grid-test.o: rs-core/grid-test.cpp rs-core/common.hpp rs-core/grid.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/index-table-test.o: rs-core/index-table-test.cpp rs-core/common.hpp rs-core/index-table.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/io-test.o: rs-core/io-test.cpp rs-core/common.hpp rs-core/io.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/io.o: rs-core/io.cpp rs-core/common.hpp rs-core/io.hpp rs-core/string.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/ipc-test.o: rs-core/ipc-test.cpp rs-core/common.hpp rs-core/ipc.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/ipc.o: rs-core/ipc.cpp rs-core/common.hpp rs-core/digest.hpp rs-core/ipc.hpp rs-core/string.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/meta-test.o: rs-core/meta-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/mp-integer-test.o: rs-core/mp-integer-test.cpp rs-core/common.hpp rs-core/mp-integer.hpp rs-core/rational.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/mp-integer.o: rs-core/mp-integer.cpp rs-core/common.hpp rs-core/mp-integer.hpp rs-core/rational.hpp rs-core/string.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/net-test.o: rs-core/net-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/net.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/net.o: rs-core/net.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/digest.hpp rs-core/net.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/optional-test.o: rs-core/optional-test.cpp rs-core/common.hpp rs-core/optional.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/process-test.o: rs-core/process-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/process.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/process.o: rs-core/process.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/io.hpp rs-core/optional.hpp rs-core/process.hpp rs-core/string.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/random-test.o: rs-core/random-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/random.o: rs-core/random.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/string.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-aggregation-test.o: rs-core/range-aggregation-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-aggregation.hpp rs-core/range-core.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-combination-test.o: rs-core/range-combination-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-combination.hpp rs-core/range-core.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-core-test.o: rs-core/range-core-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-core.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-expansion-test.o: rs-core/range-expansion-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-core.hpp rs-core/range-expansion.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-generation-test.o: rs-core/range-generation-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/range-core.hpp rs-core/range-generation.hpp rs-core/range-selection.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-permutation-test.o: rs-core/range-permutation-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-core.hpp rs-core/range-permutation.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-reduction-test.o: rs-core/range-reduction-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-core.hpp rs-core/range-permutation.hpp rs-core/range-reduction.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-selection-test.o: rs-core/range-selection-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/range-core.hpp rs-core/range-selection.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/range-transformation-test.o: rs-core/range-transformation-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/range-core.hpp rs-core/range-transformation.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/rational-test.o: rs-core/rational-test.cpp rs-core/common.hpp rs-core/rational.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/rational.o: rs-core/rational.cpp rs-core/common.hpp rs-core/rational.hpp rs-core/string.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/ring-buffer-test.o: rs-core/ring-buffer-test.cpp rs-core/common.hpp rs-core/ring-buffer.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/scale-map-test.o: rs-core/scale-map-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/scale-map.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/serial-test.o: rs-core/serial-test.cpp rs-core/blob.hpp rs-core/channel.hpp rs-core/common.hpp rs-core/compact-array.hpp rs-core/digest.hpp rs-core/file-system.hpp rs-core/float.hpp rs-core/ipc.hpp rs-core/meta.hpp rs-core/mp-integer.hpp rs-core/optional.hpp rs-core/random.hpp rs-core/rational.hpp rs-core/serial.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp rs-core/uuid.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/regex.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/serial.o: rs-core/serial.cpp rs-core/blob.hpp rs-core/channel.hpp rs-core/common.hpp rs-core/digest.hpp rs-core/encoding.hpp rs-core/file-system.hpp rs-core/float.hpp rs-core/ipc.hpp rs-core/meta.hpp rs-core/mp-integer.hpp rs-core/optional.hpp rs-core/random.hpp rs-core/rational.hpp rs-core/serial.hpp rs-core/string.hpp rs-core/time.hpp rs-core/uuid.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/regex.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/signal-test.o: rs-core/signal-test.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/signal.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/signal.o: rs-core/signal.cpp rs-core/channel.hpp rs-core/common.hpp rs-core/optional.hpp rs-core/signal.hpp rs-core/string.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/stack-test.o: rs-core/stack-test.cpp rs-core/common.hpp rs-core/stack.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/statistics-test.o: rs-core/statistics-test.cpp rs-core/common.hpp rs-core/statistics.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/string-test.o: rs-core/string-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/string.o: rs-core/string.cpp rs-core/common.hpp rs-core/string.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/table-test.o: rs-core/table-test.cpp rs-core/common.hpp rs-core/table.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/table.o: rs-core/table.cpp rs-core/common.hpp rs-core/string.hpp rs-core/table.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/terminal-test.o: rs-core/terminal-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/terminal.hpp rs-core/time.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/terminal.o: rs-core/terminal.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/terminal.hpp rs-core/time.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/thread-pool-test.o: rs-core/thread-pool-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/thread-pool.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/thread-pool.o: rs-core/thread-pool.cpp rs-core/common.hpp rs-core/string.hpp rs-core/thread-pool.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/time-test.o: rs-core/time-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/time.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/time.o: rs-core/time.cpp rs-core/common.hpp rs-core/string.hpp rs-core/time.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/topological-order-test.o: rs-core/topological-order-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/topological-order.hpp rs-core/unit-test.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/unit-test.o: rs-core/unit-test.cpp  \
    /usr/local/include/unicorn/unit-test.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/url-test.o: rs-core/url-test.cpp rs-core/common.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/url.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/url.o: rs-core/url.cpp rs-core/common.hpp rs-core/string.hpp rs-core/url.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/regex.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/uuid-test.o: rs-core/uuid-test.cpp rs-core/common.hpp rs-core/digest.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/uuid.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/uuid.o: rs-core/uuid.cpp rs-core/common.hpp rs-core/digest.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/string.hpp rs-core/uuid.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/variant-test.o: rs-core/variant-test.cpp rs-core/common.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/variant.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/vector-test.o: rs-core/vector-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/meta.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp \
    /usr/local/include/unicorn/utility.hpp
$(BUILD)/zlib-test.o: rs-core/zlib-test.cpp rs-core/common.hpp rs-core/float.hpp rs-core/io.hpp rs-core/meta.hpp rs-core/random.hpp rs-core/string.hpp rs-core/unit-test.hpp rs-core/vector.hpp rs-core/zlib.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
$(BUILD)/zlib.o: rs-core/zlib.cpp rs-core/common.hpp rs-core/io.hpp rs-core/string.hpp rs-core/zlib.hpp \
    /usr/local/include/unicorn/character.hpp /usr/local/include/unicorn/path.hpp /usr/local/include/unicorn/property-values.hpp /usr/local/include/unicorn/utf.hpp /usr/local/include/unicorn/utility.hpp
ifeq ($(LIBTAG),msvc)
    LDLIBS += unicorn.lib zlib.lib pcre2-8.lib
else
    LDLIBS += -lunicorn -lz -lpcre2-8
endif
ifeq ($(LIBTAG),cygwin)
    LDLIBS += -lcrypto
endif
ifeq ($(LIBTAG),linux)
    LDLIBS += -lcrypto
endif
ifeq ($(LIBTAG),msvc)
    LDLIBS += advapi32.lib ole32.lib shell32.lib ws2_32.lib zlib.lib
endif
