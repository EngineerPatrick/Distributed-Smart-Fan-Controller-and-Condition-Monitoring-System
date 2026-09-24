#include "atomic_operations.hpp"
#include <zephyr/sys/atomic.h>

atomic_operations::atomic32_value_t atomic_operations::atomic_var_get(atomic_operations::atomic32_t* const atomic_variable_ptr) {
	return atomic_get(atomic_variable_ptr);
}

atomic_operations::atomic32_value_t atomic_operations::atomic_var_set(atomic_operations::atomic32_t* const atomic_variable_ptr, atomic_operations::atomic32_value_t value) {
	return atomic_set(atomic_variable_ptr, value);
}
