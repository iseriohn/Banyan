# Banyan, an automatic dual-track meeting scheduler

## Description
Banyan is an automatic scheduler for running a program committee meeting in parallel tracks. 

Suppose there are ```n``` papers to be discussed and they were reviewed by ```m``` reviewers. One paper is usually reviewed by 3-5 reviewers. The meeting could be more efficient if we divide papers into two groups to be discussed in parallel tracks respectively and have the reviewers of the same paper be assigned to the same track. Meanwhile, it would be better if the numbers of papers in two tracks are as close as possible so that the meeting would take similar time in both tracks. With such expectation, it might not be feasible to construct a scheduling with given reviewing assignment. To realse the restrictions a little bit and take into account the practical fact that there could be breaks in a meeting, we could partition the meeting into several sessions and during each session, we divide the papers according to the requirements just discussed. Besides, the number of sessions should be as few as possible and the length of each session should also be similar.

Shortly put, Banyan assigns ```n```papers reviewed by ```m```reviewers into as-few-as-possible sessions and each session should consists of similar number of papers. Within each session, the papers are divided into two tracks and each track should consists of similar number of papers as well. In addition, there shall not be two papers assigned to two different tracks in the same session and they have a mutual reviewer. In other words, all the reviewers of a paper should always be in the same track where that paper is to be discussed.

## Usage
```
./run.sh <path> [-output <path>] [-maxdiff <value>] [-minsess <value>]
```

* ```./run.sh <input_file>``` must include the input file when running Banyan. Please take the format of ```example-scores.csv``` as a reference. The first row should be titles for each column and will be skipped by the program. The essential data starts from the second row and each row ends with a character ```\r```. The first column should be the index of a paper and the second column should be the title of it. Then follows the email address of a reviewer of this paper and the name of him. The four entries should be separated by comma ```,```. It's fine for a paper title to consist of a comma but not for the paper index, or the reviewer name or email.

* ```-output <output_file>``` specifies the output file for the scheduling result. It will be written in file ```result.csv``` by default. The assignment will be output session by session. And in each session, the papers discussed in each track are printed first, then follows the reviewers participating in each track. Please take ```example-result.csv``` as an example.

* ```-maxdiff <maximum_difference>``` specifies the biggest gap between the numbers of papers discussed in two tracks of the same session that could be tolerated by the committee. This parameter is set as ```1``` by default, which means that for each session, the numbers of papers discussed in the two tracks are either equal to each other or differ by 1.

* ```-minsess <minimum_sessions>``` specifies the minimum number of sessions in the assignment the committee expects. The program tries to find an assignment that has the fewest sessions, but not fewer than ```minimum_sessions```. This parameter is set to ```1``` by default, by which you could find out the optimal solution this program could construct with given data.

# Q&As
* What OS is this tool for?

Linux.

* How do I know whether the input file is in the right format and the data is parsed correctly?

Check from the logs printed on the screen whether the number of papers and reviewers are correct. If so, then everything is probably going on well!

* What if the result doesn't seem right?

Check the format of the input file. If the problem is still not solved, contact Yan.

* What if the number of tracks in the final assignment is not satisfactory?

Modify the iteration parameters in the code or contact Yan directly. There is a high probability that we need a better algorithm and there is even a higher probability that the expectation is not reachable with given data.

* How does the underlying algorithm work?

Contact Yan for more technical details or wait for Yan to write it somewhere in the future.
