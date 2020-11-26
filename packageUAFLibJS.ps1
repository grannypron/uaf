del C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js\*.*
del C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js\interop\*.*
del C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js\interop
cp C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\js\* C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js
cp C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\js\interop\* C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js\interop\
Dir C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js\*.js | rename-item -newname { [io.path]::ChangeExtension($_.name, "txt") }
Dir C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\js\interop\*.js | rename-item -newname { [io.path]::ChangeExtension($_.name, "txt") }
cp C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\data\items.xml C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\data\items.txt
cp C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\data\SpecialAbilities.xml C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\data\SpecialAbilities.txt
cp C:\Users\Shadow\Desktop\uaf.git\uaf-unity\engineManager.js C:\Users\Shadow\Desktop\uaf.git\uaf-unity\Assets\Resources\engineManager.txt




