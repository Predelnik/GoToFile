#pragma once
#ifdef _DEBUG
#define ASSERT_RETURN(CONDITION,RET) do { if (!(CONDITION)) { assert (false); return RET; }} while (0)
#else // !_DEBUG
#define ASSERT_RETURN(CONDITION,RET) do { if (!(CONDITION)) return RET; } while (0)
#endif // !_DEBUG
