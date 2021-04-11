package money

import (
	"errors"
	"fmt"
)

func NewBank() Bank {
	return Bank{rateMap: map[currencyPair]float64{}}
}

type Bank struct {
	rateMap map[currencyPair]float64
}

type currencyPair struct {
	From string
	To   string
}

func (b *Bank) AddRate(from, to Currency, rate float64) {
	pair := currencyPair{
		From: string(from),
		To:   string(to),
	}
	b.rateMap[pair] = rate
	pair = currencyPair{
		From: string(to),
		To:   string(from),
	}
	b.rateMap[pair] = 1 / rate
}

func (b *Bank) GetRate(from, to Currency) (rate float64, err error) {
	pair := currencyPair{
		From: string(from),
		To:   string(to),
	}
	rate, ok := b.rateMap[pair]
	if !ok {
		return 0.0, errors.New(fmt.Sprintf("rate not exist from %v to %v", from, to))
	}
	return rate, nil
}

func (b *Bank) Settle(exp Expression, to Currency) (res float64, err error) {
	res, err = exp.Reduce(to, b)
	return
}
