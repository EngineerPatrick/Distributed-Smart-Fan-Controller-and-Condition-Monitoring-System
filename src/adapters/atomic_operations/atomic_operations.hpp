#ifndef ATOMIC_OPERATIONS_HPP
#define ATOMIC_OPERATIONS_HPP

#include <zephyr/sys/atomic.h>

namespace atomic_operations {

	typedef atomic_t atomic32_t;
	typedef atomic_val_t atomic32_value_t;

	atomic_operations::atomic32_value_t atomic_var_get(atomic_operations::atomic32_t* const atomic_variable);
	atomic_operations::atomic32_value_t atomic_var_set(atomic_operations::atomic32_t* const atomic_variable, atomic_operations::atomic32_value_t value);
}

#endif
