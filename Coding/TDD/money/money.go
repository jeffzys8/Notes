package money

type Currency string

const (
	DollarString Currency = "dollar"
	YuanString   Currency = "yuan"
)

type Expression interface {
	Reduce(to Currency, bank *Bank) (float64, error)
}

type IMoney interface {
	Currency() Currency
	GetValue() float64
	Equal(IMoney) bool
	Times(int) IMoney
	Add(IMoney) IMoney
	Expression
}

// "abstract class" Money,
// Promise myself not to create it directly..
type Money struct {
	IMoney
	currency Currency
	value    float64
}

// promise not to directly use that.
// only as "super"
func NewMoney(v float64, c Currency) Money {
	money := Money{
		value:    v,
		currency: c,
	}
	return money
}

func NewDollar(v float64) IMoney {
	dollar := Dollar{
		Money: NewMoney(v, DollarString),
	}
	dollar.IMoney = dollar // trick to extend an "abstract class"
	return dollar
}

// TODO: dup with NewDollar
func NewYuan(v float64) IMoney {
	yuan := Yuan{
		Money: NewMoney(v, YuanString),
	}
	yuan.IMoney = yuan // trick to extend an "abstract class"
	return yuan
}

func (m Money) GetValue() float64 {
	return m.value
}

func (m Money) Currency() Currency {
	return m.currency
}

func (m Money) Equal(em IMoney) bool {
	if m.Currency() != em.Currency() {
		return false
	}
	return m.GetValue() == em.GetValue()
}
