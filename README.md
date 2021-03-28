# Electrical-RnD

To create a new project:
1. Before you begin work, pull recent updates from the repo with:
	
	`git switch master`
	Or if you're old fashioned `git checkout master`.

	Then: `git pull`

2. Create a new branch for your part with (replace NEW_BRANCH_NAME with your own _new_ branch name):
	Make the branch name something descriptive like (Pi-Shield-Parts or Molex-nanofit)
  
	`git checkout -b NEW_BRANCH_NAME`
  
3. Create a folder for your project
	This helps keep the repo clean, please do it
	Give the folder the same name as the branch (Not required but it makes things simpler)
	`mkdir FOLDERNAME`
	`cd FOLDERNAME`

4. Work on your project
	Keep all of your files inside the project folder
	Download the datasheet for any part you use and save it in the project folder

4. Getting rid of eagle's backup files:

	`git clean -n -X`  To see what files would be removed  
	`git clean -f -X`  To remove said files

5. Add your changes with:

	`git add .`

6. Commit your changes (this saves them) with (replace your message with an explanation of what was added):

	`git commit -m "YOUR MESSAGE"`
	ex: `git commit -m "Made the 4 pin molex 371/372 series miniclamp connector (part XXXX) for the backplane board."`

7. Push your changes to the remote repo with (use the same name as before):

	`git push`.
	If the branch was just created, do `git push -u origin NEW_BRANCH_NAME`
	
8. Open a pull request on [GitHub](https://github.com/purduerov/Electrical-RnD/pulls) to merge your branch into master.
