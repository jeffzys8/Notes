package money

type Yuan struct {
	value float64
}

func NewYuan(value float64) Yuan {
	return Yuan{
		value: value,
	}
}

func (y Yuan) Add(toAdd Yuan) Yuan {
	return NewYuan(y.value + toAdd.value)
}

func (y Yuan) Equal(another Yuan) bool {
	return y.value == another.value
}

func (y Yuan) Times(time int) Yuan {
	return NewYuan(y.value * float64(time))
}

func (y Yuan) Currency() Currency {
	return YuanString
}

func (y Yuan) Reduce(to Currency, bank *Bank) (float64, error) {
	if to == y.Currency() {
		return y.value, nil
	}
	rate, err := bank.GetRate(y.Currency(), to)
	if err != nil {
		return 0.0, err
	}
	return y.value * rate, nil
}
