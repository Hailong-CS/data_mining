# Import necessary libraries
import os, re, nltk, csv, math
from bs4 import BeautifulSoup, SoupStrainer
from collections import Counter
from PS import PorterStemmer

path = './reuters21578' # Initialize the path
ps = PorterStemmer() # Initialize the PorterStemmer

## Define the function to translate stoplist file content into a string
def File_to_String(file):
	stoplist = open(file, "r")
	tempstr = str(stoplist.read())
	tempstr = tempstr.replace("\n", " ")
	tempstr = ' '.join(tempstr.split())
	return tempstr


# Define the function to select the REUTERS that contain only a single topic
def One_Topic_Select(file):
	# Makine soup object and get all the REUTERS
	soup = BeautifulSoup(open(file, "r"), "html.parser")
	REUTERS_tags = soup.find_all("reuters")

	# Deduct all the REUTERS contain zero or more than one topics
	one_topic_list = list()
	for idx, element in enumerate(REUTERS_tags):
		if len(element.topics) == 1:
			one_topic_list.append(element)

	return one_topic_list


# Define the function to find topics that occur in the n most frequent
def Topic_MostFreq_Select(REUTERS_list, n):
	# Get the all topics
	topics_list = list()
	for element in REUTERS_list:
		topics_list.append(str(element.topics.next.next))
	count = Counter(topics_list)
	freq_topics = count.most_common(n)

	# Get the 20 most frequent REUTERS based on the frequent topics list
	return_list = list()
	for element_st in REUTERS_list:
		for element_nd in freq_topics:
			if str(element_st.topics.next.next) == element_nd[0]:
				return_list.append(element_st)

	return return_list


# Define the function to extract NEWID, topic and content
def Extraction(REUTERS_list):
	return_list = list()
	for element in REUTERS_list:
		temp = list()
		if element.body != None:
			temp.append(str(element.topics.next.next))
			temp.append(int(element['newid']))
			temp.append(str(element.body))
			return_list.append(temp)

	return return_list


# Define the function for sorting purpose
def Get_Key_ID(item):
	return item[1]


# Define the function to clean up the text
def Clean_up(REUTERS_list, stopstring):
	## Elininate the body tag text, \n, multiple space
	for element in REUTERS_list:
		element[2] = element[2].replace("\n", " ")
		element[2] = element[2][6:]
		element[2] = element[2][:-17]
		element[2] = ' '.join(element[2].split())
	'''
	1. Eliminate any non-ascii characters
	2. Parse character entities such as "&lt" and "&ge"
		to corresponding characters "<" and ">", respectively.
	3. Change the character case to lower-case.
    4. Replace any non alphanumeric characters with space.
    5. Split the text into tokens, using space as the delimiter.
    6. Eliminate any tokens that contain only digits.
    7. Eliminate any tokens from the stop list that is provided 
    	(file stoplist.txt).
    8. Obtain the stem of each token using Porter's stemming 
    	algorithm; you can use any of the implementations provided 
    	here: https://tartarus.org/martin/PorterStemmer/.
    9. Eliminate any tokens that occur less than 5 times.
	'''
	for element in REUTERS_list:
		element[2] = re.sub(r'[^\x00-\x7F]+', " ", element[2]) # 1.
		element[2] = element[2].replace("&lt;", "<") # 2.
		element[2] = element[2].replace("&gt;", ">") # 2.
		element[2] = element[2].lower() # 3.
		element[2] = re.sub(r'[^0-9a-zA-Z]+', " ", element[2]) # 4.

	# Get tokens as required
	tokens_word = list()
	tokens_count = list()
	tokens = list()

	for element_st in REUTERS_list:
		templist = element_st[2].split() # 5.

		temp_word = list()
		temp_count = list()
		temp = list()
		for element_nd in templist:
			if (not element_nd.isdigit()) and (not (element_nd in stopstring)): # 6. 7.
				element_nd = ps.stem(element_nd, 0, len(element_nd)-1) # 8.
				# Add the element and its overall count to overall tokens
				if element_nd in tokens_word:
					tokens_count[tokens_word.index(element_nd)] += 1
				else:
					tokens_word.append(element_nd)
					tokens_count.append(1)
				# Add the element and its count to this article
				if element_nd in temp_word:
					temp_count[temp_word.index(element_nd)] += 1
				else:
					temp_word.append(element_nd)
					temp_count.append(1)

		## Add the article tokens into the article list
		for idx, element in enumerate(temp_word):
			temp.append((element, temp_count[idx]))
		element_st.append(temp)

	temp1 = []
	for idx, element in enumerate(tokens_word):
		if tokens_count[idx] >= 5: # 9.
			tokens.append((element, tokens_count[idx]))
			temp1.append(element)
	for element_st in REUTERS_list:
		temp2 = []
		for element_nd in element_st[3]:
			if element_nd[0] in temp1:
				temp2.append(element_nd)
		element_st[3] = temp2

	return REUTERS_list, tokens


# Define the function to output class, clabel files
def Output_class_clabel(REUTERS_list, tokens_list):
	# Output "reuters21578.class" file
	class_file = open("reuters21578.class", "w")
	for element in REUTERS_list:
		class_file.write(str(element[1]) + "," + element[0] + '\n')

	# Close the file
	class_file.close()

	# Output "reuters21578.clabel" file
	clabel_file = open("reuters21578.clabel", "w")
	# Create tokens dictionary that key is dimension number and value is stem
	token_dict = {}
	for idx, element in enumerate(tokens_list):
		token_dict[element[0]] = str(idx)
		# Put the stem into file with increasing dimension number order
		clabel_file.write(str(idx) + " " + element[0] + '\n')

	# Close the file
	clabel_file.close()

	return token_dict


# Define the function to output csv, class, clabel files
def Output_csv(REUTERS_list, token_dict, approach):
	# Create the header row
	fields = ["NEWID", "Dimension number", "Value"]
	
	# Create the file name depends on the approach
	if approach == 1:
		file_name = 'freq.csv'	
	elif approach == 2:
		file_name = 'sqrtfreq.csv'
	else:
		file_name = 'log2freq.csv'

	# Write the data into file
	with open(file_name, mode = 'w') as file:
		writer = csv.DictWriter(file, delimiter = ',', quotechar = '"', 
			quoting = csv.QUOTE_MINIMAL, fieldnames = fields)
		# Write the header as created before
		writer.writeheader()
		# Using dictionary to put the corresponding value
		for element_st in REUTERS_list:
			dictionary = {}
			dictionary["NEWID"] = element_st[1]
			# Get each vector corresponding value and normalize it
			Magnitude = float(0)
			for element_nd in element_st[3]:
				if approach == 1:
					Magnitude += element_nd[1] * element_nd[1]
				elif approach == 2:
					Magnitude += float(1+math.sqrt(element_nd[1])) * float(1+math.sqrt(element_nd[1]))
				else:
					Magnitude += float(1+math.log(element_nd[1], 2)) * float(1+math.log(element_nd[1], 2))
			# Put the normalized value as dictionary
			for element_nd in element_st[3]:
				if approach == 1:
					dictionary["Dimension number"] = token_dict[element_nd[0]]
					dictionary["Value"] = element_nd[1] / Magnitude
				elif approach == 2:
					dictionary["Dimension number"] = token_dict[element_nd[0]]
					dictionary["Value"] = float(1+math.sqrt(element_nd[1])) / Magnitude
				else:
					dictionary["Dimension number"] = token_dict[element_nd[0]]
					dictionary["Value"] = float(1+math.log(element_nd[1], 2)) / Magnitude
				# Write the values into file
				writer.writerow(dictionary)

	# Close the file
	file.close()


############### Main function alike ##########################################
# Import the stoplist file and make it as a string
for files in os.listdir('.'):
	if files == "stoplist.txt":
		stopstring = File_to_String(files)

# Initialize the current working dirctory
os.chdir(path)

# Get qualified REUTERS
REUTERS = list()

# Clear all the REUTERS contain zero or more than one topics
for files in os.listdir('.'):
	if files.endswith("sgm"):
	#if files == 'reut2-000.sgm':
		REUTERS.extend(One_Topic_Select(files))

# Get the 20 most frequent REUTERS
REUTERS = Topic_MostFreq_Select(REUTERS, 20)

# Extract all the needed attributes
REUTERS = Extraction(REUTERS)

# Sort the REUTERS base on its NEWID with increasing order
REUTERS = sorted(REUTERS, key = Get_Key_ID)

# Clean the data to obtain the sparse representation
REUTERS, TOKENS = Clean_up(REUTERS, stopstring)

print(len(REUTERS))
print(len(TOKENS))

for idx, element in enumerate(TOKENS):
	if idx % 1000 == 0:
		print(element)
# Initialize the current working dirctory
os.chdir('..')

# Output class, clabel files
token_dictionary = Output_class_clabel(REUTERS, TOKENS)

# Output vector representations into csv files in 3 approaches
# Using integers to represent approaches
Output_csv(REUTERS, token_dictionary, 1)
Output_csv(REUTERS, token_dictionary, 2)
Output_csv(REUTERS, token_dictionary, 3)
