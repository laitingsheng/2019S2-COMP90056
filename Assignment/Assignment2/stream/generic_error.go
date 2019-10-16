package stream

import (
    "fmt"
)

type TerminatedStreamError {
    streamName string
}

func (*TerminatedStreamError) Error() string {
    return fmt.Sprintf("stream %s is terminated", streamName)
}

type FrequencyRangeError struct {
    frequencyMin, frequencyMax int
}

func (e, *FrequencyRangeError) Error() string {
    return fmt.Sprintf("invalid frequency range [%d, %d]", e.frequencyMin, e.frequencyMax)
}
