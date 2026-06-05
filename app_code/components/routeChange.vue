<template>
	<!-- #ifdef H5 -->
	<view class="routeChange">
		<uni-card>
				<navigator url="/pages/equipState/equipState">设备状态</navigator>
		</uni-card>
		<uni-card>
			<navigator url="/pages/equipControal/equipControal">操作界面</navigator>
		</uni-card>
		<!-- <uni-card>
			<navigator open-type="reLaunch" url="/pages/workReord/workReord">车库状态</navigator>
		</uni-card> -->
		<!-- <uni-card>
			<navigator url="/pages/findData/findData">查询数据</navigator>
		</uni-card> -->
		<uni-card>
			<navigator url="/pages/buleFind/buleFind">蓝牙巡检</navigator>
		</uni-card>
		<!-- <uni-card>
			<navigator url="/pages/addPeole/addPeole">人员注册</navigator>
		</uni-card> -->
	</view>
	<!-- #endif -->
</template>

<script>
	export default {
		name:"routeChange",
		props:{
			num:{
				type:Number
			}
		},
		data() {
			return {
			};
		},
		methods:{
		}
	}
</script>

<style>
.routeChange{
	display: flex;
	text-align: center;
}
.active{
	background-color: beige;
}
</style>