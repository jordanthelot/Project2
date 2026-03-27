#EloFinder

#Purpose

Lets you search for a football club and view it's elo history from 2000-2025. Elo history is taken twice a month, resulting
in over 200 pieces of data for each club. Also has the functionality for a top 10 leaderboard, which is a top 10 of the club's highest
elo ratings. A historical line graph is also included. Data is taken from 27 countires and 42 leagues, primarily from Europe. 

#Requirements
-VSCode with Live Server extension
-Python 3
-g++ compiler

#How to Run 

1. Clone the folder in your terminal/ repository

2. Navigate to the path where your folder is located

3. Compile the file-
g++ -std=c++17 -o server functions.c++

4. Start the Python server
python3 server.py

5. Before using the go live feature, you can see what clubs are availible to search by using:
cut -d',' -f2 EloRatings.csv | sort -u

6. Use the Go live feature in bottom right corner when on firstpage.html to start using the website


#How to Use
Type a football club name into the search box

The results page shows the most recent elo rating, and dropdown menu with all the elo ratings dating back to 2000

Click "SEE TOP 10 ELO RATINGS" to view the club's highest all time elo ratings.

