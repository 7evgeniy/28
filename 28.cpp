/*
 * Задача: дана произвольная последовательность из 19 неповторяющхся чисел от 1 до 19.
 * Дана характеристика этой последовательности: наименьшее значение суммы трёх соседних чисел.
 * Последовательность кольцевая, то есть первое число и последнее число суть соседние.
 * Какое наибольшее значение может принимать эта характеристика?
 *
 * Математическими средствами доказывается, что это не 29 и не какое-нибудь большее число.
 * Предположим, что всякая сумма трёх соседних чисел в последовательности не меньше 29.
 * Докажем отсюда, что всякое число в последовательности не больше 16.
 * Возьмём число на произвольной позиции. Все остальные числа — это 6 групп по три числа.
 * Сумма всех остальных чисел не меньше 29*6 = 174. Сумма же всех чисел строго равна 190.
 * Отсюда следует, что произвольно взятое число не больше 16, но это абсурд.
 * Следовательно, во всякой последовательности есть хотя бы одна сумма, меньшая 29.
 *
 * Задача для перебора: найти такую последовательность, в которой всякая сумма
 * трёх соседних чисел не меньше 28.
 */

#include <limits>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>

typedef std::vector<int> IntVector;

/*
 * Объект этого класса, видоизменяя заданную последовательность _nums,
 * находит улучшенную последовательность перестановками элементов.
 */
class Search {
public:
	Search (IntVector &nums);
	int maximise ();
	const IntVector& nums () const;
	int sum (int index);
	static const int SeqLen;
private:
	bool select_step ();
	bool valid_step (int e1, int e2);
	void make_step (int e1, int e2);
	void calculate_sums ();
public:         // каждое число в последовательности и каждая сумма обозначается индексом:
	static int inc (int index);    // вернуть следующий индекс
	static int dec (int index);    // вернуть предыдущий индекс
	static bool belongs (int element, int sum);   // истина, если число входит в сумму
private:
	IntVector _nums;    // числа в последовательности — по индексам
	IntVector _sums;    // суммы: предыдущее число + это число + следующее число
	int _min;               // индекс наименьшей суммы
};

int calc_min_sum (const IntVector& v) {
	IntVector s;
	s.resize(v.size());
	for (int i = 0; i < v.size(); ++i)
		s[i] = v[Search::dec(i)]+v[i]+v[Search::inc(i)];
	return *std::min_element(s.begin(), s.end());
}


const int MinNum = 28;
const int Search::SeqLen = 19;

Search::Search (IntVector &nums) : _nums(nums) {}
const IntVector& Search::nums () const {return _nums;}

int Search::inc (int index) {
	return (index+1) % SeqLen;
}

int Search::dec (int index) {
	return index>0 ? index-1 : SeqLen-1;
}

bool Search::belongs (int element, int sum) {
	return sum == element || inc(sum) == element || dec(sum) == element;
}

int Search::sum (int index) {
	return _nums[dec(index)] + _nums[index] + _nums[inc(index)];
}

// Нельзя обменивать, если какая-нибудь сумма после обмена достигает величины _sums[_min].
bool Search::valid_step (int e1, int e2) {
	int diff = _nums[e2] - _nums[e1];
	for (int e = dec(e2); belongs(e2, e); e = inc(e))
		if (sum(e)-diff <= _sums[_min])
			return false;
	for (int e = dec(e1); belongs(e1, e); e = inc(e))
		if (sum(e)+diff <= _sums[_min])
			return false;
	return true;
}

// Процедура возвращает «истину», если смогла увеличить наименьшую сумму за счёт обмена элементов.
// Процедура выбирает среди всех перспективных обменов наиболее радикальный и делает именно его.
bool Search::select_step () {
	int el1 = -1, el2 = -1;
	int fall = 0;   // всегда неположительное число
	for (int e1 = dec(_min); belongs(e1, _min); e1 = inc(e1))
		for (int e2 = 0; e2 < SeqLen; ++ e2)
			if (valid_step(e1, e2) && fall > _nums[e1]-_nums[e2]) {
				fall = _nums[e1]-_nums[e2];
				el1 = e1; el2 = e2;
			}
	if (el1 == -1 || el2 == -1)
		return false;
	make_step(el1, el2);
	return true;
}

void Search::make_step (int e1, int e2) {
	int element = _nums[e1];
	_nums[e1] = _nums[e2];
	_nums[e2] = element;
}

void Search::calculate_sums () {
	_sums.resize(SeqLen);
	for (int i = 0; i < SeqLen; ++ i)
		_sums[i] = sum(i);
	_min = std::min_element(_sums.begin(), _sums.end()) - _sums.begin();
}

int Search::maximise () {
	do {
		calculate_sums();
	} while (select_step());
	return _sums[_min];
}

// истина, если два вектора обозначают одну и ту же кольцевую последовательность.
bool same (const IntVector &one, const IntVector &two) {
	if (one.size() != two.size())
		return false;
	int i1 = std::find(one.begin(), one.end(), Search::SeqLen) - one.begin();
	int i2 = std::find(two.begin(), two.end(), Search::SeqLen) - two.begin();
	if (i1 == Search::SeqLen || i2 == Search::SeqLen)
		return false;
	int forward = i2, backward = i2;
	bool good1 = true, good2 = true;
	for (int c = 0; c < Search::SeqLen-1; ++ c) {
		++ i1;
		if (good1 && (i1 != ++forward))
			good1 = false;
		if (good2 && (i1 != --backward))
			good2 = false;
		if (!good1 && !good2)
			return false;
	}
	return true;
}

// истина, если кольцевая последовательность nums не входит в список последовательностей list.
bool unique (const std::vector<IntVector> &list, const IntVector &nums) {
	for (const IntVector &v : list)
		if (same(v, nums))
			return false;
	return true;
}

const int MaxAttemptCount = 1000;

// напечатать список из нескольких правильных последовательностей, удовлетворяющих условиям задачи.
int main () {
	std::random_device rd;
	std::default_random_engine g(rd());
	std::vector<IntVector> list;
	for (int c = 0; c < MaxAttemptCount; ++ c) {
		IntVector nums;        // взять для оптимизации случайную последовательнось:
		for (int i = 1; i <= Search::SeqLen; ++ i)
			nums.push_back(i);
		std::shuffle(nums.begin(), nums.end(), g);
		Search search(nums);
		if (search.maximise() == MinNum && unique(list, search.nums()))  // оптимизировать её;
			list.push_back(search.nums());   // и если получена новая правильная
	}                                                // последовательность, то записать её.
	for (const IntVector &v : list) {
		for (int n : v)
			std::cout << n << ' ';
		std::cout << "    " << calc_min_sum(v) << std::endl;
	}
	if (list.empty())
		std::cerr << "Последовательности не найдены\n";
	return 0;
}
