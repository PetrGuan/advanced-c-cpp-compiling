# How Does a Database Work?

1. What format is data saved in? (in memory and on disk)
2. When does it move from memory to disk?
3. Why can there only be one primary key per table?
4. How does rolling back a transaction work?
5. How are indexes formatted?
6. When and how does a full table scan happen?
7. What format is a prepared statement saved in?

I’m building a clone of sqlite from scratch in C in order to understand, and I’m going to document my process as I go.
