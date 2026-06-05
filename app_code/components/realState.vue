<template>
	<view class="cardList">
		<uni-card title="设备状态" thumbnail="../../static/images/deviceControl.png">
			<uni-card :title="equip.name" style="width:22%;flex: none;">
				<image :src="equip.icon" mode="widthFix" style="width:70%;"></image><br>
				{{equip.state}}
			</uni-card>
		</uni-card>
	</view>
</template>
<script>
	export default {
		data() {
			return {

			}
		}
	}
</script>

<style>
	.cardList {
		display: flex;
		flex-wrap: wrap;
		justify-content: center;
		text-align: center;
	}
</style>
