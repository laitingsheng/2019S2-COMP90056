package integer

import (
    "fmt"
)

type ValueRangeError struct {
    valueMin, valueMax int
}

func (e *ValueRangeError) Error() string {
    return fmt.Sprintf("invalid value range [%d, %d]", e.valueMin, e.valueMax)
}
