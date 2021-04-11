package money

type Currency string

const (
	DollarString Currency = "dollar"
	YuanString   Currency = "yuan"
)

type Expression interface {
	Reduce(to Currency, bank *Bank) (float64, error)
}

type Money interface {
	Currency() string
}
