# CS50, Tiny Search Engine (TSE)
## Ahmed Al Sunbati
## Overview
This Tiny Search Engine (TSE) consists of three subsystems that work together to crawl websites, index the content, and allow users to search through the collected data. Each subsystem is a standalone program executed from the command line, and they interact through files stored in the file system.
### Crawler
The TSE crawler is a standalone program that crawls the web and retrieves webpages starting from a "seed" URL. It parses the seed webpage, extracts any embedded URLs, then retrieves each of those pages, recursively, but limiting its exploration to a given "depth".

> For more information check crawler/README.md

### Indexer
The TSE indexer is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file. Its companion, the index tester, loads an index file produced by the indexer and saves it to another file.

> For more information check indexer/README.md

### Querier
The TSE Querier is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin.

> For more information check querier/README.md