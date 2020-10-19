class test1:
    def test_func1(self):
        print("In function1")

    def test_func2(self):
        print("In function2")
        self.test_func1()

t1 = test1()
t1.test_func2()
