package integer

import (
    "math/rand"
    "time"
)

type Insertion struct {
    r, b, f int
    rv, rf rand.Rand
}

func (s *Insertion) Next() (int, int) {
    return s.rv.Intn(s.r)-s.b, s.rf.Intn(s.f)
}

func NewIntegerInsertionStream(valueMin, valueMax, frequencyMax int) (*Insertion, error) {
    if valueMin > valueMax {
        return nil, &ValueRangeError{valueMin, valueMax}
    }
    if frequencyMax < 0 {
        return nil, &stream.FrequencyRangeError{0, frequencyMax}
    }
    return &Insertion{valueMax-valueMin+1, valueMin, frequencyMax+1, rand.New(rand.NewSource(time.Now().UTC().UnixNano())), rand.New(rand.NewSource(time.Now().Unix()))}, nil
}
