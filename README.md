Реализован шаблонный класс односвязного списка SingleLinkedList<Type> со следующим функционалом:
- Конструктор по умолчанию, который создаёт пустой список;
- метод GetSize, который возвращает количество элементов в списке;
- метод IsEmpty, который возвращает true, если список пустой, и false в противном случае.
- операция PushFront, которая делает вставки элемента в начало односвязного списка
Метод PushFront предоставляет строгую гарантию безопасности исключений: если в процессе работы метода будет выброшено исключение, состояние списка будет таким же, как до вызова метода.
- Метод Clear очищает список и не выбрасывает исключений.
- При разрушении списка удаляются все его элементы.
- Операции сравнения ==, !=, <, >, <=, >=;
- Обмен содержимого двух списков с использованием метода swap и шаблонной функции swap;
- Конструирование односвязного списка на основе initializer_list. Последовательность элементов созданного списка и initializer_list одинаковая;
- Надёжные конструктор копирования и операция присваивания. Операция присваивания обеспечивает строгую гарантию безопасности исключений. Если в процессе присваивания будет выброшено исключение, содержимое левого аргумента операции присваивания остается без изменений.
- Метод PopFront. Удаляет первый элемента непустого списка за время O(1). Не выбрасывает исключений.
- Метод InsertAfter. За время O(1) вставляет в список новое значение следом за элементом, на который ссылается переданный в InsertAfter итератор. Метод обеспечивает строгую гарантию безопасности исключений.
- Метод EraseAfter. За время O(1) удаляет из списка элемент, следующий за элементом, на который ссылается переданный в InsertAfter итератор. Не выбрасывает исключений.
- Методы before_begin и cbefore_begin. Возвращают итераторы, ссылающиеся на фиктивную позицию перед первым элементом списка. Такой итератор используется как параметр для методов InsertAfter и EraseAfter, когда нужно вставить или удалить элемент в начале списка. Разыменовывать этот итератор нельзя.

Реализован шаблонный класс BasicIterator, на основе которого будут объявлены константный и неконстантный итераторы списка.
- В классе списка реализованы константная и неконстантная версии методов begin и end, которые возвращают итераторы на первый элемент контейнера и позицию, следующую за последним элементом.
- Чтобы получать константные итераторы было удобно, реализованы методы cbegin и cend.
- Операции сравнения ==, !=, <, >, <=, >=;

  Также добавлены тесты для проверки правильной работы списка
