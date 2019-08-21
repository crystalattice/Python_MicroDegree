class CallTracer:
    def __init__(self, func):
        """# On initial call, save original func"""
        self.calls = 0
        self.func = func

    def __call__(self, *args):
        """# On later calls, run original func"""
        self.calls += 1
        print(f"Call {self.calls} to function '{self.func.__name__}'")
        self.func(*args)  # Collect all arguments


@CallTracer
def spam(*args):
    """Add or concatenate any number of arguments"""
    for arg in args:
        if type(arg) is int:
            x = 0
        elif type(arg) is str:
            x = ""
        else:
            print("Invalid input type")
        x = x + arg
        print(x, end="")
    print("\n")


if __name__ == "__main__":
    spam(1, 2, 3)
    spam("a", "b", "c")
    spam(9, 8, 7, 6, 5)
    spam(1.0, 2.0)
    print(f"Number of function calls: {spam.calls}")
