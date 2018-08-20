#pragma once

#include "../ProcBackend2/Tests.h"

namespace Frontend {
	int start() {
		Tests::test_all();
		return 0;
	}
}
