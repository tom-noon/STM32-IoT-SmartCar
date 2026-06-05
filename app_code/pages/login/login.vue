<template>
	<view class="login">
		<!-- 图文卡片模式 -->
		<uni-card title="登录物联网小车后台" style="width: 50%;margin:200rpx auto;text-align: center;">
				<uni-easyinput focus v-model="username" placeholder="请输入用户名"></uni-easyinput><br>
				<uni-easyinput focus type="password" v-model="password" placeholder="请输入密码"></uni-easyinput><br>
				<button type="primary" @click="login">登录</button>
		</uni-card>
	</view>
</template>

<script>
	import myAlert from '@/utils/myAlert.js'
	export default {
		data() {
			return{
				username:"admin",
				password:"admin"
			}
		},
		methods: {
			login(){
				
				if(this.username==="admin"&&this.password==="admin"){
					// #ifdef H5
					alert("登录成功")
					uni.navigateTo({
						url:"/pages/equipState/equipState"
					})
					// #endif
					// #ifdef APP
					uni.showToast({
						title:"登录成功"
					})
					uni.switchTab({
						url:"/pages/equipState/equipState"
					})
					// #endif
					
				}else{
					myAlert('账号或密码错误')
				}
			}
		}
	}
</script>

<style scoped lang="scss">
	.login{
		height: 100%;
	}
</style>