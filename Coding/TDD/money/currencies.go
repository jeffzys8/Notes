package money

const (
	DollarString Currency = "dollar"
	YuanString   Currency = "yuan"
)

func NewDollar(v float64) Money {
	return NewMoney(v, DollarString)
}

func NewYuan(v float64) Money {
	return NewMoney(v, YuanString)
}
