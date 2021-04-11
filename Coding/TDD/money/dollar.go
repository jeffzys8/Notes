package money

type Dollar struct {
	value float64
}

func NewDollar(value float64) Dollar {
	return Dollar{
		value: value,
	}
}

func (d Dollar) Add(toAdd Dollar) Dollar {
	return NewDollar(d.value + toAdd.value)
}

func (d Dollar) Equal(another Dollar) bool {
	return d.value == another.value
}

func (d Dollar) Times(time int) Dollar {
	return NewDollar(d.value * float64(time))
}

func (d Dollar) Currency() Currency {
	return DollarString
}

func (y Dollar) Reduce(to Currency, bank *Bank) (float64, error) {
	if to == y.Currency() {
		return y.value, nil
	}
	rate, err := bank.GetRate(y.Currency(), to)
	if err != nil {
		return 0.0, err
	}
	return y.value * rate, nil
}
