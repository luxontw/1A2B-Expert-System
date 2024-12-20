# 1A2B-Expert-System
在本系統中，程式產生1A2B遊戲5040組可能密碼後，便會開始模擬5040組密碼的猜測情形。每組密碼的猜測流程包括：計算較能排除最多答案的問題(首次猜測均為0123)、獲得提示、排除答案，重複循環直到找出正確密碼。系統中提示為程式自行判讀產生，並藉由C++ Async平行化處理加快產出所有密碼猜測次數。
## GeneratePossibleAnswers
- C++ Template 實作泛型，可產生不同字元與長度密碼
- 使用DFS產生所有組合再以C++ next_permutation排列
## CalculateBestQuestion
- 為了降低猜測回合數，在猜測前依序取出剩餘可能密碼中的每一組，與剩餘密碼做1A2B關係的比對，模擬0A0B～4A0B 14種可能提示的分類情形，找出能將剩餘可能密碼分類的最平均的猜測，如此能降低每一種提示組別中的成員數量，獲得提示後，可能解僅會剩下提示組別中的某一組，便能有相對較高的機率排除最多答案
## ExcludeAnswers
- 如猜測與答案的關係為1A1B，答案與猜測的關係也為1A1B
- 利用此性質，如剩餘可能密碼與猜測關係、答案與猜測關係兩者不同，即可排除此答案
## Compute1～Compute10
- 以C++ Async平行化處理5040種密碼的猜測情形，單一Compute模擬約500組密碼，以更好的利用CPU資源，加快產出結果的速度
## GetHint
- 比對猜測與密碼關係
## Main()
- 顯示各猜測次數統計結果與平均猜測次數
