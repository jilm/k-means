# k-means
Just another k-means algorithm implementation, this time in C.

I have needed something to play with. First time I tryed to implement it in
python, but it was really slow. Yes, I know that it was probably my fault, 
I doesn't blame python.

## Learning Mode

In this mode the training set and the number of clusters \(K\) must be given:

> k-means -k 3 --learn < data.txt

## Clasification Mode

Onece the centroids are known, data may be simply classified like this:

> k-means --classify --centroids centr.txt < data.txt

## Analyze Mode

This mode is useful in cases we know little about the data to say how many clusters contain:

> k-means --analyze < data.txt

## Another Useful Options

* --debug
* --verbose

## Input data file structure

It is simply the text file, vector delimiter is a new line character and number delimiter inside the vector is a single space.
