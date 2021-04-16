package money

type Yuan struct {
	Money
}

func (y Yuan) Add(toAdd IMoney) IMoney {
	return NewYuan(y.GetValue() + toAdd.GetValue())
}

func (y Yuan) Times(time int) IMoney {
	return NewYuan(y.GetValue() * float64(time))
}

func (y Yuan) Reduce(to Currency, bank *Bank) (float64, error) {
	if to == y.Currency() {
		return y.GetValue(), nil
	}
	rate, err := bank.GetRate(y.Currency(), to)
	if err != nil {
		return 0.0, err
	}
	return y.GetValue() * rate, nil
}
