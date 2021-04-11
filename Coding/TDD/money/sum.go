package money

type Sum struct {
	Add   Expression
	ToAdd Expression
}

func NewSum(add, toAdd Expression) Sum {
	return Sum{
		Add:   add,
		ToAdd: toAdd,
	}
}

func (s Sum) Reduce(to Currency, bank *Bank) (float64, error) {
	left, err := s.Add.Reduce(to, bank)
	if err != nil {
		return 0, err
	}
	right, err := s.ToAdd.Reduce(to, bank)
	if err != nil {
		return 0, err
	}
	return left + right, nil
}
