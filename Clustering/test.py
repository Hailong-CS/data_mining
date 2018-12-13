def Get_Key(item):
	return item[1]
old = [["hhh", 8], ["hao", 100], ["sab", 50], ["niub", 120], ["gre", 150]]

old = sorted(old, key = Get_Key)
print(old)

