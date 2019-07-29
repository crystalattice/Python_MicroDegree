from math import pi


def arg_checker(input_func):
    def wrapper(num):
        if type(num) != float:
            raise TypeError("Argument is not a float")
        elif num <= 0:
            raise ValueError("Argument is not positive")
        else:
            return input_func(num)
    return wrapper


@arg_checker
def circle_measures(radius):
    circumference = 2 * pi * radius
    area = pi * radius * radius
    diameter = 2 * radius

    return diameter, circumference, area


if __name__ == "__main__":
    diameter, circumference, area = circle_measures(6.0)
    print("The diameter is {diam}. \nThe circumference is {circum}. \nThe area is {area}".format(
        diam=diameter, circum = circumference, area=area))
