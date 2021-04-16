package money

import "fmt"

type Currency string

type Expression interface {
	Reduce(to Currency, bank *Bank) (float64, error)
}

// now is time to make it not abstract
type Money struct {
	currency Currency
	value    float64
	Expression
}

func NewMoney(v float64, c Currency) Money {
	money := Money{
		value:    v,
		currency: c,
	}
	return money
}

func (m Money) GetValue() float64 {
	return m.value
}

func (m Money) Currency() Currency {
	return m.currency
}

func (m Money) Equal(em Money) bool {
	if m.Currency() != em.Currency() {
		return false
	}
	return m.GetValue() == em.GetValue()
}

func (m Money) Times(time int) Money {
	return NewMoney(m.GetValue()*float64(time), m.currency)
}

func (m Money) Add(toAdd Money) (Money, error) {
	if m.Currency() != toAdd.Currency() {
		return Money{}, fmt.Errorf("Adding %v to %v", m.Currency(), toAdd.Currency())
	}
	return NewMoney(m.GetValue()+toAdd.GetValue(), DollarString), nil
}

func (m Money) Reduce(to Currency, bank *Bank) (float64, error) {
	if to == m.Currency() {
		return m.GetValue(), nil
	}
	rate, err := bank.GetRate(m.Currency(), to)
	if err != nil {
		return 0.0, err
	}
	return m.GetValue() * rate, nil
}
