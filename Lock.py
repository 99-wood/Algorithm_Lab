import hashlib
import os

class PasswordLock:
    def __init__(self):
        self.salt = b'\xb2S"e}\xdf\xb0\xfe\x9c\xde\xde\xfe\xf3\x1d\xdc>'
    
    def hash_password(self, password):            
        # 将密码转换为字节流
        password_bytes = password.encode('utf-8')
        
        # 将盐值和密码组合并进行哈希
        hash_obj = hashlib.sha256(self.salt + password_bytes)
        password_hash = hash_obj.hexdigest()
        
        return password_hash
    
    def verify_password(self, input_password, stored_hash):
        # 使用相同的盐值对输入密码进行哈希
        calculated_hash = self.hash_password(input_password)
        
        # 比较计算出的哈希值与存储的哈希值
        return calculated_hash == stored_hash


# 使用示例
if __name__ == "__main__":
    lock = PasswordLock()
    
    # 用户设置密码
    user_password = input("请设置一个三位数密码: ")
    
    # 加密密码
    password_hash = lock.hash_password(user_password)
    print(f"密码已加密，哈希值: {password_hash}")
    
    # 验证密码
    test_password = input("请输入密码进行验证: ")
    if lock.verify_password(test_password, password_hash):
        print("密码验证成功！")
    else:
        print("密码验证失败！")
