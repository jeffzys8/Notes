package money

type Dollar struct {
	Money
}

func (d Dollar) Add(toAdd IMoney) IMoney {
	// if d.Currency() != toAdd.Currency() {
	// 	return Dollar{}, fmt.Errorf("Adding %v to %v", d.Currency(), toAdd.Currency())
	// }
	return NewDollar(d.GetValue() + toAdd.GetValue())
}

func (d Dollar) Times(time int) IMoney {
	return NewDollar(d.GetValue() * float64(time))
}

func (y Dollar) Reduce(to Currency, bank *Bank) (float64, error) {
	if to == y.Currency() {
		return y.GetValue(), nil
	}
	rate, err := bank.GetRate(y.Currency(), to)
	if err != nil {
		return 0.0, err
	}
	return y.GetValue() * rate, nil
}
