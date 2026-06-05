
<!-- not use -->

<template>
	<view style="
	width: 32%;
	<!-- #ifndef H5 -->
		width: 100%;
	<!-- #endif -->
	">
		<uni-card :title="`姓名:${cardInfo.name}`" thumbnail="../../static/images/people.png">
				<h1>卡号 : {{cardInfo.card_num}}</h1><br>
				<h1>余额 : {{cardInfo.balance}}</h1><br>
				<h1>状态 : {{cardInfo.work_state}}</h1>
				<!-- <button @click="detailInfo">详细信息</button> -->
		</uni-card>
	</view>
</template>

<script>
	export default {
		name:"detailCard",
		props:{
			cardInfo:{
				type:Object
			}
		},
		data() {
			return {
				
			};
		},	
		methods:{
			detailInfo(){
				uni.navigateTo({
					url:`/pages/carDetail/carDetail?id=${this.carId}`,
					
				})
			}
		}
	}
</script>

<style>

</style>