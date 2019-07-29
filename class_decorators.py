class Dog:
    def __init__(self, breed, age):
        self.breed = breed
        self.age = age

    @property
    def dog_age(self):
        return self.age

    @dog_age.setter
    def dog_age(self, age):
        self.age = age

    @property
    def dog_breed(self):
        return self.breed

    @dog_breed.setter
    def dog_breed(self, breed):
        self.breed = breed

    @staticmethod
    def noise():
        return "Woof Woof"

    @classmethod
    def dog_type(cls):
        if cls.__name__ == "Dog":
            return "A domestic mutt"
        else:
            return cls.__name__

    def __repr__(self):
        """Return string representation of Dog object.

        Without this, only the object's memory address will be printed.
        """
        return f"{self.breed}, {self.age}"


class Shiba(Dog):
    pass


if __name__ == "__main__":
    holmes = Dog("Mini Sheltie", 3)
    print(holmes.dog_type())

    koko = Shiba("Cream", 3)
    print(koko.dog_type())
