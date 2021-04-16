package money_test

import (
	"testing"

	"example.com/money"
	"github.com/stretchr/testify/assert"
)

func TestCurrency(t *testing.T) {
	assert.Equal(t, money.DollarString, money.NewDollar(1).Currency())
	assert.Equal(t, money.YuanString, money.NewYuan(1).Currency())
}

func TestEquality(t *testing.T) {
	five := money.NewDollar(5)
	anotherFive := money.NewDollar(5)
	fiveYuan := money.NewYuan(5)
	assert.True(t, five.Equal(anotherFive))
	assert.False(t, five.Equal(fiveYuan))
}

func TestTimes(t *testing.T) {
	twoAndHalf := money.NewDollar(2.5)
	five := money.NewDollar(5.0)

	assert.True(t, five.Equal(twoAndHalf.Times(2)))
}

func TestAdd(t *testing.T) {
	two := money.NewDollar(2)
	three := money.NewDollar(3)
	five := money.NewDollar(5)
	res, err := two.Add(three)
	assert.Nil(t, err)
	assert.True(t, five.Equal(res))
	res, err = two.Add(money.NewYuan(3))
	assert.NotNil(t, err)
}

func TestYuanTimes(t *testing.T) {
	twoAndHalf := money.NewYuan(2.5)
	five := money.NewYuan(5.0)

	assert.True(t, five.Equal(twoAndHalf.Times(2)))
}

func TestDifferentAdd(t *testing.T) {
	fiveYuan := money.NewYuan(5)
	fiveDollar := money.NewDollar(5)
	money.NewSum(fiveYuan, fiveDollar)
}

func TestBank(t *testing.T) {
	bank := money.NewBank()
	bank.AddRate(money.DollarString, money.YuanString, 6.5)
	rate, err := bank.GetRate(money.DollarString, money.YuanString)
	assert.Nil(t, err)
	assert.Equal(t, 6.5, rate)

	rate, err = bank.GetRate(money.YuanString, money.DollarString)
	assert.Nil(t, err)
	assert.Equal(t, 1/6.5, rate)
}

func TestReduce(t *testing.T) {
	bank := money.NewBank()
	bank.AddRate(money.DollarString, money.YuanString, 6.5)

	Yuan13 := money.NewYuan(6.5).Times(2)
	res, err := Yuan13.Reduce(money.DollarString, &bank)
	assert.Nil(t, err)
	assert.Equal(t, float64(2), res)
}

func TestBankSettle(t *testing.T) {
	Yuan26 := money.NewYuan(13).Times(2)
	Dollar5 := money.NewDollar(5)
	sum := money.NewSum(Yuan26, Dollar5)
	bank := money.NewBank()
	bank.AddRate(money.DollarString, money.YuanString, 6.5)

	result, err := bank.Settle(sum, money.DollarString)
	assert.Nil(t, err)
	assert.Equal(t, float64(9), result)

	result, err = bank.Settle(sum, money.YuanString)
	assert.Nil(t, err)
	assert.Equal(t, float64(9*6.5), result)

}
