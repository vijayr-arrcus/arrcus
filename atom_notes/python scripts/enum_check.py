# Python code to demonstrate enumerations

# importing enum for enumerations
import enum
print(enum.__file__)
# creating enumerations using class
class Animal(enum.Enum):
    dog = 1
    cat = 2
    lion = 3

# printing enum member as string
print ("The string representation of enum member is : ")
print (Animal.dog)

# printing enum member as repr
print ("The repr representation of enum member is : ")
print (repr(Animal.dog))

# printing the type of enum member using type()
print ("The type of enum member is : ")
print (type(Animal.dog))

# printing name of enum member using "name" keyword
print ("The name of enum member is : ")
print (Animal.dog.name)



class IntfCounterType(enum.Enum):
    Unicast   = 1
    Multicast = 2
    Broadcast = 3

class ActionType(enum.Enum):
    Accept = 1
    Deny   = 2

print(IntfCounterType.Unicast)
