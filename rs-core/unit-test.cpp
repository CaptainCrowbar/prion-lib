#include "rs-core/unit-test.hpp"

extern void test_core_common_accountable();
extern void test_core_common_preprocessor_macros();
extern void test_core_common_integer_types();
extern void test_core_common_string_types();
extern void test_core_common_metaprogramming();
extern void test_core_common_type_adapters();
extern void test_core_common_type_related_functions();
extern void test_core_common_version_number();
extern void test_core_common_arithmetic_literals();
extern void test_core_common_generic_algorithms();
extern void test_core_common_integer_sequences();
extern void test_core_common_memory_algorithms();
extern void test_core_common_range_traits();
extern void test_core_common_range_types();
extern void test_core_common_generic_arithmetic();
extern void test_core_common_integer_arithmetic();
extern void test_core_common_bitwise_operations();
extern void test_core_common_function_traits();
extern void test_core_common_function_operations();
extern void test_core_common_generic_function_objects();
extern void test_core_common_hash_functions();
extern void test_core_common_scope_guards();
extern void test_core_algorithm_diff();
extern void test_core_algorithm_edit_distance();
extern void test_core_algorithm_find_optimum();
extern void test_core_algorithm_paired_for_each();
extern void test_core_algorithm_paired_sort();
extern void test_core_algorithm_paired_transform();
extern void test_core_array_map_class();
extern void test_core_array_map_set();
extern void test_core_blob_class();
extern void test_core_cache_class();
extern void test_core_channel_true();
extern void test_core_channel_false();
extern void test_core_channel_generator();
extern void test_core_channel_buffer();
extern void test_core_channel_queue();
extern void test_core_channel_value();
extern void test_core_channel_timer();
extern void test_core_channel_throttle();
extern void test_core_channel_polled();
extern void test_core_channel_dispatcher();
extern void test_core_compact_array_construction();
extern void test_core_compact_array_capacity();
extern void test_core_compact_array_insertion();
extern void test_core_compact_array_accounting();
extern void test_core_compact_array_keys();
extern void test_core_digest_utility_functions();
extern void test_core_digest_adler32();
extern void test_core_digest_crc32();
extern void test_core_digest_siphash24();
extern void test_core_digest_md5();
extern void test_core_digest_sha1();
extern void test_core_digest_sha256();
extern void test_core_digest_sha512();
extern void test_core_encoding_escape();
extern void test_core_encoding_quote();
extern void test_core_encoding_hex();
extern void test_core_encoding_base32();
extern void test_core_encoding_base64();
extern void test_core_encoding_ascii85();
extern void test_core_encoding_z85();
extern void test_core_file_names();
extern void test_core_file_system_queries();
extern void test_core_file_system_update();
extern void test_core_file_io();
extern void test_core_file_metadata();
extern void test_core_file_standard_locations();
extern void test_core_float_arithmetic_constants();
extern void test_core_float_arithmetic_functions();
extern void test_core_float_arithmetic_literals();
extern void test_core_float_line_integral();
extern void test_core_float_volume_integral();
extern void test_core_float_precision_sum();
extern void test_core_float_root_finding_parameters();
extern void test_core_float_bisection();
extern void test_core_float_false_position();
extern void test_core_float_newton_raphson();
extern void test_core_float_pseudo_newton_raphson();
extern void test_core_grid_class();
extern void test_core_index_table_classes();
extern void test_core_io_cstdio();
extern void test_core_io_fdio();
extern void test_core_io_pipe();
extern void test_core_io_winio();
extern void test_core_io_null_device();
extern void test_core_io_external_buffer();
extern void test_core_io_internal_buffer();
extern void test_core_io_print_formatting();
extern void test_core_ipc_named_mutex();
extern void test_core_kwargs_keyword_arguments();
extern void test_core_meta_logic();
extern void test_core_meta_append();
extern void test_core_meta_concat();
extern void test_core_meta_insert();
extern void test_core_meta_insert_at();
extern void test_core_meta_most();
extern void test_core_meta_prefix();
extern void test_core_meta_rep_list();
extern void test_core_meta_rep_type();
extern void test_core_meta_resize();
extern void test_core_meta_skip();
extern void test_core_meta_sublist();
extern void test_core_meta_tail();
extern void test_core_meta_take();
extern void test_core_meta_head();
extern void test_core_meta_last();
extern void test_core_meta_max_type();
extern void test_core_meta_type_at();
extern void test_core_meta_fold();
extern void test_core_meta_make_set();
extern void test_core_meta_map();
extern void test_core_meta_partial_reduce();
extern void test_core_meta_remove();
extern void test_core_meta_reverse();
extern void test_core_meta_select();
extern void test_core_meta_sort();
extern void test_core_meta_unique();
extern void test_core_meta_zip();
extern void test_core_meta_inherit();
extern void test_core_meta_tuples();
extern void test_core_meta_all_of();
extern void test_core_meta_count();
extern void test_core_meta_find();
extern void test_core_meta_in_list();
extern void test_core_meta_is_empty();
extern void test_core_meta_is_unique();
extern void test_core_meta_length_of();
extern void test_core_meta_operator_detection();
extern void test_core_meta_function_detection();
extern void test_core_meta_type_categories();
extern void test_core_meta_related_types();
extern void test_core_mp_integer_unsigned_conversion();
extern void test_core_mp_integer_unsigned_arithmetic();
extern void test_core_mp_integer_unsigned_bit_operations();
extern void test_core_mp_integer_unsigned_byte_operations();
extern void test_core_mp_integer_unsigned_random();
extern void test_core_mp_integer_signed_conversion();
extern void test_core_mp_integer_signed_arithmetic();
extern void test_core_mp_integer_signed_random();
extern void test_core_mp_integer_literals();
extern void test_core_mp_integer_core_functions();
extern void test_core_mp_integer_rational_basics();
extern void test_core_mp_integer_rational_reduction();
extern void test_core_mp_integer_rational_arithmetic();
extern void test_core_mp_integer_rational_properties();
extern void test_core_mp_integer_rational_comparison();
extern void test_core_mp_integer_rational_mixed();
extern void test_core_mp_integer_rational_parsing();
extern void test_core_net_ipv4();
extern void test_core_net_ipv6();
extern void test_core_net_socket_address();
extern void test_core_net_ip_literals();
extern void test_core_net_dns_query();
extern void test_core_net_tcp_client_server();
extern void test_core_net_socket_set();
extern void test_core_optional_basic_operations();
extern void test_core_optional_object_accounting();
extern void test_core_optional_comparison_operators();
extern void test_core_optional_coalescing_operators();
extern void test_core_optional_function_calls();
extern void test_core_process_shell_commands();
extern void test_core_process_stream();
extern void test_core_process_text();
extern void test_core_random_lcg();
extern void test_core_random_xoroshiro();
extern void test_core_random_basic_distributions();
extern void test_core_random_sample();
extern void test_core_random_triangular();
extern void test_core_random_uniform_integer_distribution();
extern void test_core_random_binomial_distribution();
extern void test_core_random_dice_distribution();
extern void test_core_random_uniform_real_distribution();
extern void test_core_random_normal_distribution();
extern void test_core_random_vectors();
extern void test_core_random_spherical_distributions();
extern void test_core_random_unique_distribution();
extern void test_core_random_weighted_choice_distribution();
extern void test_core_random_algorithms();
extern void test_core_random_text_generators();
extern void test_core_rational_basics();
extern void test_core_rational_reduction();
extern void test_core_rational_arithmetic();
extern void test_core_rational_properties();
extern void test_core_rational_comparison();
extern void test_core_rational_mixed();
extern void test_core_rational_parsing();
extern void test_skeleton_ring_buffer_basics();
extern void test_skeleton_ring_buffer_accounting_with_copy();
extern void test_skeleton_ring_buffer_accounting_with_move();
extern void test_core_scale_map_class();
extern void test_core_serial_standard_types();
extern void test_core_serial_std_chrono();
extern void test_core_serial_blob();
extern void test_core_serial_compact_array();
extern void test_core_serial_endian();
extern void test_core_serial_file();
extern void test_core_serial_multiprecision_integers();
extern void test_core_serial_optional();
extern void test_core_serial_rational();
extern void test_core_serial_uuid();
extern void test_core_serial_vector_matrix_quaternion();
extern void test_core_serial_version();
extern void test_core_serial_persistent_storage();
extern void test_core_signal_channel();
extern void test_core_signal_name();
extern void test_core_stack_class();
extern void test_core_statistics_class();
extern void test_core_string_literals();
extern void test_core_string_character();
extern void test_core_string_property();
extern void test_core_string_manipulation();
extern void test_core_string_formatting();
extern void test_core_string_parsing();
extern void test_core_string_html_xml_tags();
extern void test_core_string_type_names();
extern void test_core_string_unicode();
extern void test_core_table_class();
extern void test_core_terminal_progress_bar();
extern void test_core_terminal_io_operations();
extern void test_core_time_date_types();
extern void test_core_time_convert_time_point();
extern void test_core_time_general_operations();
extern void test_core_time_format_date();
extern void test_core_time_format_time();
extern void test_core_time_parse_date();
extern void test_core_time_parse_time();
extern void test_core_time_backoff_wait();
extern void test_core_time_system_specific_conversions();
extern void test_core_topological_order_algorithm();
extern void test_core_topological_order_reverse();
extern void test_skeleton_url_parse_http();
extern void test_skeleton_url_parse_file();
extern void test_skeleton_url_parse_mailto();
extern void test_skeleton_url_assembly();
extern void test_skeleton_url_modifiers();
extern void test_skeleton_url_navigation();
extern void test_skeleton_url_encoding();
extern void test_skeleton_url_query();
extern void test_core_uuid_class();
extern void test_core_variant_types();
extern void test_core_variant_behaviour();
extern void test_core_variant_function_call();
extern void test_core_variant_comparison();
extern void test_core_vector_integer();
extern void test_core_vector_floating();
extern void test_core_vector_matrix();
extern void test_core_vector_matrix_inversion();
extern void test_core_vector_quaternion();
extern void test_core_vector_2d_coordinate_transformations();
extern void test_core_vector_3d_coordinate_transformations();
extern void test_core_vector_projective();
extern void test_core_vector_primitives();
extern void test_core_vector_quaternions();
extern void test_core_zlib_compression();
extern void test_core_zlib_io();

int main() {

    static const RS::UnitTest::test_index index = {

        { "core/common/accountable", test_core_common_accountable },
        { "core/common/preprocessor-macros", test_core_common_preprocessor_macros },
        { "core/common/integer-types", test_core_common_integer_types },
        { "core/common/string-types", test_core_common_string_types },
        { "core/common/metaprogramming", test_core_common_metaprogramming },
        { "core/common/type-adapters", test_core_common_type_adapters },
        { "core/common/type-related-functions", test_core_common_type_related_functions },
        { "core/common/version-number", test_core_common_version_number },
        { "core/common/arithmetic-literals", test_core_common_arithmetic_literals },
        { "core/common/generic-algorithms", test_core_common_generic_algorithms },
        { "core/common/integer-sequences", test_core_common_integer_sequences },
        { "core/common/memory-algorithms", test_core_common_memory_algorithms },
        { "core/common/range-traits", test_core_common_range_traits },
        { "core/common/range-types", test_core_common_range_types },
        { "core/common/generic-arithmetic", test_core_common_generic_arithmetic },
        { "core/common/integer-arithmetic", test_core_common_integer_arithmetic },
        { "core/common/bitwise-operations", test_core_common_bitwise_operations },
        { "core/common/function-traits", test_core_common_function_traits },
        { "core/common/function-operations", test_core_common_function_operations },
        { "core/common/generic-function-objects", test_core_common_generic_function_objects },
        { "core/common/hash-functions", test_core_common_hash_functions },
        { "core/common/scope-guards", test_core_common_scope_guards },
        { "core/algorithm/diff", test_core_algorithm_diff },
        { "core/algorithm/edit-distance", test_core_algorithm_edit_distance },
        { "core/algorithm/find-optimum", test_core_algorithm_find_optimum },
        { "core/algorithm/paired-for-each", test_core_algorithm_paired_for_each },
        { "core/algorithm/paired-sort", test_core_algorithm_paired_sort },
        { "core/algorithm/paired-transform", test_core_algorithm_paired_transform },
        { "core/array-map/class", test_core_array_map_class },
        { "core/array-map/set", test_core_array_map_set },
        { "core/blob/class", test_core_blob_class },
        { "core/cache/class", test_core_cache_class },
        { "core/channel/true", test_core_channel_true },
        { "core/channel/false", test_core_channel_false },
        { "core/channel/generator", test_core_channel_generator },
        { "core/channel/buffer", test_core_channel_buffer },
        { "core/channel/queue", test_core_channel_queue },
        { "core/channel/value", test_core_channel_value },
        { "core/channel/timer", test_core_channel_timer },
        { "core/channel/throttle", test_core_channel_throttle },
        { "core/channel/polled", test_core_channel_polled },
        { "core/channel/dispatcher", test_core_channel_dispatcher },
        { "core/compact-array/construction", test_core_compact_array_construction },
        { "core/compact-array/capacity", test_core_compact_array_capacity },
        { "core/compact-array/insertion", test_core_compact_array_insertion },
        { "core/compact-array/accounting", test_core_compact_array_accounting },
        { "core/compact-array/keys", test_core_compact_array_keys },
        { "core/digest/utility-functions", test_core_digest_utility_functions },
        { "core/digest/adler32", test_core_digest_adler32 },
        { "core/digest/crc32", test_core_digest_crc32 },
        { "core/digest/siphash24", test_core_digest_siphash24 },
        { "core/digest/md5", test_core_digest_md5 },
        { "core/digest/sha1", test_core_digest_sha1 },
        { "core/digest/sha256", test_core_digest_sha256 },
        { "core/digest/sha512", test_core_digest_sha512 },
        { "core/encoding/escape", test_core_encoding_escape },
        { "core/encoding/quote", test_core_encoding_quote },
        { "core/encoding/hex", test_core_encoding_hex },
        { "core/encoding/base32", test_core_encoding_base32 },
        { "core/encoding/base64", test_core_encoding_base64 },
        { "core/encoding/ascii85", test_core_encoding_ascii85 },
        { "core/encoding/z85", test_core_encoding_z85 },
        { "core/file/names", test_core_file_names },
        { "core/file/system-queries", test_core_file_system_queries },
        { "core/file/system-update", test_core_file_system_update },
        { "core/file/io", test_core_file_io },
        { "core/file/metadata", test_core_file_metadata },
        { "core/file/standard-locations", test_core_file_standard_locations },
        { "core/float/arithmetic-constants", test_core_float_arithmetic_constants },
        { "core/float/arithmetic-functions", test_core_float_arithmetic_functions },
        { "core/float/arithmetic-literals", test_core_float_arithmetic_literals },
        { "core/float/line-integral", test_core_float_line_integral },
        { "core/float/volume-integral", test_core_float_volume_integral },
        { "core/float/precision-sum", test_core_float_precision_sum },
        { "core/float/root-finding-parameters", test_core_float_root_finding_parameters },
        { "core/float/bisection", test_core_float_bisection },
        { "core/float/false-position", test_core_float_false_position },
        { "core/float/newton-raphson", test_core_float_newton_raphson },
        { "core/float/pseudo-newton-raphson", test_core_float_pseudo_newton_raphson },
        { "core/grid/class", test_core_grid_class },
        { "core/index-table/classes", test_core_index_table_classes },
        { "core/io/cstdio", test_core_io_cstdio },
        { "core/io/fdio", test_core_io_fdio },
        { "core/io/pipe", test_core_io_pipe },
        { "core/io/winio", test_core_io_winio },
        { "core/io/null-device", test_core_io_null_device },
        { "core/io/external-buffer", test_core_io_external_buffer },
        { "core/io/internal-buffer", test_core_io_internal_buffer },
        { "core/io/print-formatting", test_core_io_print_formatting },
        { "core/ipc/named-mutex", test_core_ipc_named_mutex },
        { "core/kwargs/keyword-arguments", test_core_kwargs_keyword_arguments },
        { "core/meta/logic", test_core_meta_logic },
        { "core/meta/append", test_core_meta_append },
        { "core/meta/concat", test_core_meta_concat },
        { "core/meta/insert", test_core_meta_insert },
        { "core/meta/insert-at", test_core_meta_insert_at },
        { "core/meta/most", test_core_meta_most },
        { "core/meta/prefix", test_core_meta_prefix },
        { "core/meta/rep-list", test_core_meta_rep_list },
        { "core/meta/rep-type", test_core_meta_rep_type },
        { "core/meta/resize", test_core_meta_resize },
        { "core/meta/skip", test_core_meta_skip },
        { "core/meta/sublist", test_core_meta_sublist },
        { "core/meta/tail", test_core_meta_tail },
        { "core/meta/take", test_core_meta_take },
        { "core/meta/head", test_core_meta_head },
        { "core/meta/last", test_core_meta_last },
        { "core/meta/max-type", test_core_meta_max_type },
        { "core/meta/type-at", test_core_meta_type_at },
        { "core/meta/fold", test_core_meta_fold },
        { "core/meta/make-set", test_core_meta_make_set },
        { "core/meta/map", test_core_meta_map },
        { "core/meta/partial-reduce", test_core_meta_partial_reduce },
        { "core/meta/remove", test_core_meta_remove },
        { "core/meta/reverse", test_core_meta_reverse },
        { "core/meta/select", test_core_meta_select },
        { "core/meta/sort", test_core_meta_sort },
        { "core/meta/unique", test_core_meta_unique },
        { "core/meta/zip", test_core_meta_zip },
        { "core/meta/inherit", test_core_meta_inherit },
        { "core/meta/tuples", test_core_meta_tuples },
        { "core/meta/all-of", test_core_meta_all_of },
        { "core/meta/count", test_core_meta_count },
        { "core/meta/find", test_core_meta_find },
        { "core/meta/in-list", test_core_meta_in_list },
        { "core/meta/is-empty", test_core_meta_is_empty },
        { "core/meta/is-unique", test_core_meta_is_unique },
        { "core/meta/length-of", test_core_meta_length_of },
        { "core/meta/operator-detection", test_core_meta_operator_detection },
        { "core/meta/function-detection", test_core_meta_function_detection },
        { "core/meta/type-categories", test_core_meta_type_categories },
        { "core/meta/related-types", test_core_meta_related_types },
        { "core/mp-integer/unsigned-conversion", test_core_mp_integer_unsigned_conversion },
        { "core/mp-integer/unsigned-arithmetic", test_core_mp_integer_unsigned_arithmetic },
        { "core/mp-integer/unsigned-bit-operations", test_core_mp_integer_unsigned_bit_operations },
        { "core/mp-integer/unsigned-byte-operations", test_core_mp_integer_unsigned_byte_operations },
        { "core/mp-integer/unsigned-random", test_core_mp_integer_unsigned_random },
        { "core/mp-integer/signed-conversion", test_core_mp_integer_signed_conversion },
        { "core/mp-integer/signed-arithmetic", test_core_mp_integer_signed_arithmetic },
        { "core/mp-integer/signed-random", test_core_mp_integer_signed_random },
        { "core/mp-integer/literals", test_core_mp_integer_literals },
        { "core/mp-integer/core-functions", test_core_mp_integer_core_functions },
        { "core/mp-integer/rational-basics", test_core_mp_integer_rational_basics },
        { "core/mp-integer/rational-reduction", test_core_mp_integer_rational_reduction },
        { "core/mp-integer/rational-arithmetic", test_core_mp_integer_rational_arithmetic },
        { "core/mp-integer/rational-properties", test_core_mp_integer_rational_properties },
        { "core/mp-integer/rational-comparison", test_core_mp_integer_rational_comparison },
        { "core/mp-integer/rational-mixed", test_core_mp_integer_rational_mixed },
        { "core/mp-integer/rational-parsing", test_core_mp_integer_rational_parsing },
        { "core/net/ipv4", test_core_net_ipv4 },
        { "core/net/ipv6", test_core_net_ipv6 },
        { "core/net/socket-address", test_core_net_socket_address },
        { "core/net/ip-literals", test_core_net_ip_literals },
        { "core/net/dns-query", test_core_net_dns_query },
        { "core/net/tcp-client-server", test_core_net_tcp_client_server },
        { "core/net/socket-set", test_core_net_socket_set },
        { "core/optional/basic-operations", test_core_optional_basic_operations },
        { "core/optional/object-accounting", test_core_optional_object_accounting },
        { "core/optional/comparison-operators", test_core_optional_comparison_operators },
        { "core/optional/coalescing-operators", test_core_optional_coalescing_operators },
        { "core/optional/function-calls", test_core_optional_function_calls },
        { "core/process/shell-commands", test_core_process_shell_commands },
        { "core/process/stream", test_core_process_stream },
        { "core/process/text", test_core_process_text },
        { "core/random/lcg", test_core_random_lcg },
        { "core/random/xoroshiro", test_core_random_xoroshiro },
        { "core/random/basic-distributions", test_core_random_basic_distributions },
        { "core/random/sample", test_core_random_sample },
        { "core/random/triangular", test_core_random_triangular },
        { "core/random/uniform-integer-distribution", test_core_random_uniform_integer_distribution },
        { "core/random/binomial-distribution", test_core_random_binomial_distribution },
        { "core/random/dice-distribution", test_core_random_dice_distribution },
        { "core/random/uniform-real-distribution", test_core_random_uniform_real_distribution },
        { "core/random/normal-distribution", test_core_random_normal_distribution },
        { "core/random/vectors", test_core_random_vectors },
        { "core/random/spherical-distributions", test_core_random_spherical_distributions },
        { "core/random/unique-distribution", test_core_random_unique_distribution },
        { "core/random/weighted-choice-distribution", test_core_random_weighted_choice_distribution },
        { "core/random/algorithms", test_core_random_algorithms },
        { "core/random/text-generators", test_core_random_text_generators },
        { "core/rational/basics", test_core_rational_basics },
        { "core/rational/reduction", test_core_rational_reduction },
        { "core/rational/arithmetic", test_core_rational_arithmetic },
        { "core/rational/properties", test_core_rational_properties },
        { "core/rational/comparison", test_core_rational_comparison },
        { "core/rational/mixed", test_core_rational_mixed },
        { "core/rational/parsing", test_core_rational_parsing },
        { "skeleton/ring-buffer/basics", test_skeleton_ring_buffer_basics },
        { "skeleton/ring-buffer/accounting-with-copy", test_skeleton_ring_buffer_accounting_with_copy },
        { "skeleton/ring-buffer/accounting-with-move", test_skeleton_ring_buffer_accounting_with_move },
        { "core/scale-map/class", test_core_scale_map_class },
        { "core/serial/standard-types", test_core_serial_standard_types },
        { "core/serial/std-chrono", test_core_serial_std_chrono },
        { "core/serial/blob", test_core_serial_blob },
        { "core/serial/compact-array", test_core_serial_compact_array },
        { "core/serial/endian", test_core_serial_endian },
        { "core/serial/file", test_core_serial_file },
        { "core/serial/multiprecision-integers", test_core_serial_multiprecision_integers },
        { "core/serial/optional", test_core_serial_optional },
        { "core/serial/rational", test_core_serial_rational },
        { "core/serial/uuid", test_core_serial_uuid },
        { "core/serial/vector-matrix-quaternion", test_core_serial_vector_matrix_quaternion },
        { "core/serial/version", test_core_serial_version },
        { "core/serial/persistent-storage", test_core_serial_persistent_storage },
        { "core/signal/channel", test_core_signal_channel },
        { "core/signal/name", test_core_signal_name },
        { "core/stack/class", test_core_stack_class },
        { "core/statistics/class", test_core_statistics_class },
        { "core/string/literals", test_core_string_literals },
        { "core/string/character", test_core_string_character },
        { "core/string/property", test_core_string_property },
        { "core/string/manipulation", test_core_string_manipulation },
        { "core/string/formatting", test_core_string_formatting },
        { "core/string/parsing", test_core_string_parsing },
        { "core/string/html-xml-tags", test_core_string_html_xml_tags },
        { "core/string/type-names", test_core_string_type_names },
        { "core/string/unicode", test_core_string_unicode },
        { "core/table/class", test_core_table_class },
        { "core/terminal/progress-bar", test_core_terminal_progress_bar },
        { "core/terminal/io-operations", test_core_terminal_io_operations },
        { "core/time/date-types", test_core_time_date_types },
        { "core/time/convert-time-point", test_core_time_convert_time_point },
        { "core/time/general-operations", test_core_time_general_operations },
        { "core/time/format-date", test_core_time_format_date },
        { "core/time/format-time", test_core_time_format_time },
        { "core/time/parse-date", test_core_time_parse_date },
        { "core/time/parse-time", test_core_time_parse_time },
        { "core/time/backoff-wait", test_core_time_backoff_wait },
        { "core/time/system-specific-conversions", test_core_time_system_specific_conversions },
        { "core/topological-order/algorithm", test_core_topological_order_algorithm },
        { "core/topological-order/reverse", test_core_topological_order_reverse },
        { "skeleton/url/parse-http", test_skeleton_url_parse_http },
        { "skeleton/url/parse-file", test_skeleton_url_parse_file },
        { "skeleton/url/parse-mailto", test_skeleton_url_parse_mailto },
        { "skeleton/url/assembly", test_skeleton_url_assembly },
        { "skeleton/url/modifiers", test_skeleton_url_modifiers },
        { "skeleton/url/navigation", test_skeleton_url_navigation },
        { "skeleton/url/encoding", test_skeleton_url_encoding },
        { "skeleton/url/query", test_skeleton_url_query },
        { "core/uuid/class", test_core_uuid_class },
        { "core/variant/types", test_core_variant_types },
        { "core/variant/behaviour", test_core_variant_behaviour },
        { "core/variant/function-call", test_core_variant_function_call },
        { "core/variant/comparison", test_core_variant_comparison },
        { "core/vector/integer", test_core_vector_integer },
        { "core/vector/floating", test_core_vector_floating },
        { "core/vector/matrix", test_core_vector_matrix },
        { "core/vector/matrix-inversion", test_core_vector_matrix_inversion },
        { "core/vector/quaternion", test_core_vector_quaternion },
        { "core/vector/2d-coordinate-transformations", test_core_vector_2d_coordinate_transformations },
        { "core/vector/3d-coordinate-transformations", test_core_vector_3d_coordinate_transformations },
        { "core/vector/projective", test_core_vector_projective },
        { "core/vector/primitives", test_core_vector_primitives },
        { "core/vector/quaternions", test_core_vector_quaternions },
        { "core/zlib/compression", test_core_zlib_compression },
        { "core/zlib/io", test_core_zlib_io },

    };

    return RS::UnitTest::test_main(index);

}
