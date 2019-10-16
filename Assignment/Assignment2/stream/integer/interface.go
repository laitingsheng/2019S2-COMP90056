package integer

type Stream interface {
    Next() (int, int, error)
}
