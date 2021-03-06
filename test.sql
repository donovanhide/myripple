CREATE DATABASE IF NOT EXISTS myripple;
USE myripple;
DROP FUNCTION IF EXISTS amount;
CREATE FUNCTION amount RETURNS string SONAME 'myripple.so';
DROP FUNCTION IF EXISTS sum_amount;
CREATE AGGREGATE FUNCTION sum_amount RETURNS string SONAME 'myripple.so';
DROP TEMPORARY TABLE IF EXISTS tests;
CREATE TEMPORARY TABLE tests (num BINARY(8),expected VARCHAR(512),display BOOLEAN);
INSERT INTO tests VALUES
(UNHEX('8000000000000000'),"0",1),
(UNHEX('D585543DF729C000'),"15000",1),
(UNHEX('95038D7EA4C68000'),"-100",1),
(UNHEX('D493B9C519CC0780'),"5.55228075",1),
(UNHEX('954A84A9A4B32B57'),"-2960.613914651479",1),
(UNHEX('D4438D7EA4C68000'),"0.1",1),
(UNHEX('D45FF973CAFA8000'),"0.9",1),
(UNHEX('D448E1BC9BF04000'),"0.25",1),
(UNHEX('D3CD801472258000'),"0.0038",1),
(UNHEX('9358E3351854DF78'),"-0.00007005216622043",1),
(UNHEX('D408E1BC9BF04000'),"0.025",1),
(UNHEX('FFFFFFFFFFFFFFFF'),"1801439850948198300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",0),
(UNHEX('C000000000000001'),"0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",0);
SELECT hex(num),amount(num),expected,IF(amount(num)<>expected,"Fail","Pass")as Test FROM tests WHERE display;
SELECT IF(COUNT(*)>0,"Failed","Passed") AS SimpleTests FROM tests WHERE amount(num)<>expected;
-- SELECT sum_amount(num) FROM tests;